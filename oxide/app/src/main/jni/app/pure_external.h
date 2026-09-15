#pragma once
#include "memory.h"
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

// Pure external - NO INJECT, NO FRIDA, only process_vm_readv + /proc/pid/maps
// For BlueStacks with adb root + setenforce 0

struct MapRegion {
    uintptr_t start;
    uintptr_t end;
    uintptr_t size;
    std::string perms;
    std::string path;
};

class PureExternal {
public:
    Memory* mem;
    uintptr_t il2cppBase = 0;
    uintptr_t il2cppEnd = 0;
    std::vector<MapRegion> regions;

    PureExternal(Memory* m) : mem(m) {
        il2cppBase = m->get_il2cpp_base();
        parseMaps();
    }

    void parseMaps() {
        char path[64];
        snprintf(path, sizeof(path), "/proc/%d/maps", mem->pid);
        FILE* fp = fopen(path, "r");
        if (!fp) return;
        char line[1024];
        while (fgets(line, sizeof(line), fp)) {
            MapRegion r;
            char perms[8] = {0};
            char mapPath[512] = {0};
            uintptr_t start, end;
            if (sscanf(line, "%lx-%lx %s %*s %*s %*s %s", &start, &end, perms, mapPath) >=3) {
                r.start = start;
                r.end = end;
                r.size = end - start;
                r.perms = perms;
                r.path = mapPath;
                regions.push_back(r);
                if (strstr(line, "libil2cpp.so")) {
                    if (il2cppBase == 0) il2cppBase = start;
                    il2cppEnd = end;
                }
            }
        }
        fclose(fp);
    }

    // Read string from remote process
    std::string readString(uintptr_t addr, size_t maxLen = 64) {
        if (!addr) return "";
        char buf[128] = {0};
        mem->readv(addr, buf, maxLen);
        buf[maxLen-1] = 0;
        return std::string(buf);
    }

    // Check if pointer is valid (in any mapped region)
    bool isValidPtr(uintptr_t ptr) {
        if (ptr < 0x10000) return false;
        for (auto& r : regions) {
            if (ptr >= r.start && ptr < r.end) return true;
        }
        return false;
    }

    // Find Il2CppClass for PlayerManager by scanning s_TypeInfo array
    // TypeDefIndex for PlayerManager = 9223 from dump.cs
    uintptr_t findPlayerManagerClass() {
        const int targetIndex = 9223;
        const char* targetName = "PlayerManager";

        // Find libil2cpp.so data region (rw-p)
        for (auto& r : regions) {
            if (r.path.find("libil2cpp.so") == std::string::npos) continue;
            if (r.perms.find("rw") == std::string::npos) continue;
            if (r.size < 0x10000) continue;

            // Read region in chunks
            size_t chunkSize = 0x10000;
            std::vector<char> buffer(chunkSize);
            
            for (uintptr_t addr = r.start; addr < r.end; addr += chunkSize) {
                size_t toRead = std::min(chunkSize, (size_t)(r.end - addr));
                if (!mem->readv(addr, buffer.data(), toRead)) continue;

                // Look for array of pointers where index 9223 points to PlayerManager class
                // We scan for pointer that when dereferenced as Il2CppClass has name == PlayerManager
                for (size_t offset = 0; offset + targetIndex*8 + 8 < toRead; offset += 8) {
                    uintptr_t candidateArrayBase = addr + offset;
                    uintptr_t classPtrAddr = candidateArrayBase + targetIndex*8;
                    
                    // Check if classPtrAddr is within our buffer
                    if (classPtrAddr < addr || classPtrAddr + 8 > addr + toRead) continue;
                    
                    uintptr_t klass = *(uintptr_t*)(buffer.data() + (classPtrAddr - addr));
                    if (!isValidPtr(klass)) continue;

                    // Read Il2CppClass name pointer at klass + 0x10
                    uintptr_t namePtr = mem->getPtr(klass + 0x10);
                    if (!isValidPtr(namePtr)) continue;

                    std::string name = readString(namePtr, 32);
                    if (name == targetName) {
                        // Found s_TypeInfo base!
                        LOGD("Found PlayerManager class at %lx, s_TypeInfo base at %lx", klass, candidateArrayBase);
                        return klass;
                    }
                }
            }
        }
        return 0;
    }

    // Get static fields from Il2CppClass
    uintptr_t getStaticFields(uintptr_t klass) {
        if (!klass) return 0;
        // Try common offsets for static_fields in Il2CppClass
        // For Unity 2019-2021, it's at 0xB8, 0xC0, 0xC8
        uintptr_t offsets[] = {0xB8, 0xC0, 0xC8, 0xB0, 0xD0};
        for (auto off : offsets) {
            uintptr_t sf = mem->getPtr(klass + off);
            if (isValidPtr(sf) && sf > 0x1000) {
                // Validate: static fields should contain pointers to List objects
                // Check if at sf+0x10 there's a valid pointer (clientPlayerList)
                uintptr_t test = mem->getPtr(sf + 0x10);
                if (isValidPtr(test) || test == 0) {
                    return sf;
                }
            }
        }
        return 0;
    }

    // Parse List<PlayerManager>
    std::vector<uintptr_t> parseList(uintptr_t listAddr) {
        std::vector<uintptr_t> result;
        if (!listAddr) return result;
        if (!isValidPtr(listAddr)) return result;

        int size = mem->getInt(listAddr + 0x18);
        if (size <=0 || size > 500) return result;

        uintptr_t items = mem->getPtr(listAddr + 0x10);
        if (!isValidPtr(items)) return result;

        int maxLength = mem->getInt(items + 0x18);
        if (maxLength < size || maxLength > 1000) return result;

        for (int i=0; i<size; i++) {
            uintptr_t player = mem->getPtr(items + 0x20 + i*8);
            if (!isValidPtr(player)) continue;
            // Validate PlayerManager: check lastTickPosition at 0x1C8 is reasonable Vector3
            float x = mem->getFloat(player + 0x1C8);
            float y = mem->getFloat(player + 0x1C8 + 4);
            float z = mem->getFloat(player + 0x1C8 + 8);
            if (isnan(x) || isnan(y) || isnan(z)) continue;
            if (fabs(x) > 10000 || fabs(y) > 10000 || fabs(z) > 10000) continue;
            result.push_back(player);
        }
        return result;
    }

    // Main function: get players without any injection
    std::vector<uintptr_t> getPlayersPureExternal() {
        std::vector<uintptr_t> result;

        // Method 1: Try to find PlayerManager class via s_TypeInfo scanning
        uintptr_t klass = findPlayerManagerClass();
        if (klass) {
            uintptr_t staticFields = getStaticFields(klass);
            if (staticFields) {
                // clientPlayerList at 0x10, active at 0x8
                uintptr_t clientList = mem->getPtr(staticFields + 0x10);
                uintptr_t activeList = mem->getPtr(staticFields + 0x8);

                auto players = parseList(clientList);
                if (!players.empty()) {
                    LOGD("PureExternal: got %zu players from clientList %lx", players.size(), clientList);
                    return players;
                }
                players = parseList(activeList);
                if (!players.empty()) {
                    LOGD("PureExternal: got %zu players from activeList %lx", players.size(), activeList);
                    return players;
                }
            }
        }

        // Method 2: Brute force scan for List objects in heap
        // Scan all rw regions for List that contains PlayerManager-like objects
        LOGD("PureExternal: brute force scanning for player lists");
        for (auto& r : regions) {
            if (r.perms.find("rw") == std::string::npos) continue;
            if (r.size > 0x1000000) continue; // skip huge regions
            if (r.size < 0x1000) continue;

            // Only scan anon heap and [heap] regions for speed
            if (r.path.find("[heap]") == std::string::npos && 
                r.path.find("[anon") == std::string::npos &&
                r.path.find("libil2cpp") == std::string::npos) continue;

            size_t toRead = r.size;
            if (toRead > 0x50000) toRead = 0x50000; // limit per region for speed

            std::vector<char> buffer(toRead);
            if (!mem->readv(r.start, buffer.data(), toRead)) continue;

            for (size_t off = 0; off + 0x20 < toRead; off += 8) {
                uintptr_t listAddr = r.start + off;
                int size = *(int*)(buffer.data() + off + 0x18);
                if (size <=0 || size > 100) continue;

                uintptr_t items = *(uintptr_t*)(buffer.data() + off + 0x10);
                if (!isValidPtr(items)) continue;

                // Quick check: items should be in heap and have maxLength >= size
                // We need to read items header
                int maxLen = mem->getInt(items + 0x18);
                if (maxLen < size || maxLen > 200) continue;

                // Try to parse
                auto players = parseList(listAddr);
                if (players.size() >= 1 && players.size() <= 100) {
                    // Additional validation: check if players have distinct positions
                    if (players.size() > 1) {
                        float x1 = mem->getFloat(players[0] + 0x1C8);
                        float x2 = mem->getFloat(players[1] + 0x1C8);
                        if (fabs(x1 - x2) < 0.1f) continue; // same pos, likely not player list
                    }
                    LOGD("PureExternal: found list at %lx with %zu players via brute force", listAddr, players.size());
                    return players;
                }
            }
        }

        LOGD("PureExternal: no players found");
        return result;
    }

private:
    void LOGD(const char* fmt, ...) {
        char buf[512];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buf, sizeof(buf), fmt, args);
        va_end(args);
        __android_log_print(ANDROID_LOG_DEBUG, "PureExternal", "%s", buf);
    }
};
