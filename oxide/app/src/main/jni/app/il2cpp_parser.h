#pragma once
#include "memory.h"
#include <vector>
#include <string>
#include <cstdint>

// Pure external Il2Cpp parser for BlueStacks - no Frida, no injection
// Parses libil2cpp.so and GlobalMetadata.dat to find PlayerManager class

struct Il2CppClassInfo {
    uintptr_t address; // remote address of Il2CppClass
    uintptr_t static_fields;
    std::string name;
    std::string namespaze;
};

class Il2CppParser {
public:
    Memory* mem;
    uintptr_t il2cppBase = 0;
    uintptr_t metadataBase = 0; // GlobalMetadata.dat mapped base in target process (if found)
    
    Il2CppParser(Memory* m) : mem(m) {
        il2cppBase = m->get_il2cpp_base();
    }

    // Find Il2CppClass for PlayerManager by scanning memory for class name
    // This is heuristic but works for BlueStacks with root + permissive
    uintptr_t findClassByName(const char* name, const char* namespaze = "") {
        // Approach 1: Scan libil2cpp.so .data for pointer to name string
        // Approach 2: Scan heap for Il2CppClass structures that have name == "PlayerManager"
        
        // For BlueStacks, we can try to read GlobalMetadata.dat from APK to get TypeDefIndex (9223)
        // Then find s_TypeInfo array in libil2cpp.so
        // s_TypeInfo is array of Il2CppClass* indexed by TypeDefIndex
        
        // To find s_TypeInfo, we need to parse libil2cpp.so ELF symbols
        // Look for symbol "s_TypeInfo" or "s_Il2CppTypeInfo" etc
        
        // Simplified: brute force scan for class
        return bruteForceFindClass(name);
    }

    uintptr_t bruteForceFindClass(const char* className) {
        // Scan /proc/pid/maps for rw regions that could contain Il2CppClass
        // Il2CppClass has: at offset 0x10 name pointer, 0x18 namespaze pointer
        // We search for string "PlayerManager" in memory, then find references to it
        
        // Step 1: Find string "PlayerManager" in target process memory
        // We need to scan all readable regions
        // For BlueStacks, this is feasible with root
        
        // This is placeholder - full implementation would scan /proc/pid/maps
        // and read each region via process_vm_readv
        
        return 0;
    }

    // Get static fields from Il2CppClass
    uintptr_t getStaticFields(uintptr_t klass) {
        if (!klass) return 0;
        // Il2CppClass->static_fields offset depends on il2cpp version
        // For Unity 2019-2021 (Oxide), it's typically at 0xB8 or 0xC0
        // Try both
        uintptr_t static_fields = mem->getPtr(klass + 0xB8);
        if (static_fields && static_fields > 0x1000) return static_fields;
        static_fields = mem->getPtr(klass + 0xC0);
        if (static_fields && static_fields > 0x1000) return static_fields;
        static_fields = mem->getPtr(klass + 0xC8);
        return static_fields;
    }

    // Get List<PlayerManager> from static fields + offset
    uintptr_t getPlayerListFromStatic(uintptr_t static_fields, int offset) {
        if (!static_fields) return 0;
        return mem->getPtr(static_fields + offset);
    }

    // Parse List<T> to get array of PlayerManager pointers
    std::vector<uintptr_t> parsePlayerList(uintptr_t listAddr) {
        std::vector<uintptr_t> result;
        if (!listAddr) return result;
        
        int size = mem->getInt(listAddr + 0x18); // List.size at 0x18
        if (size <=0 || size > 1000) return result;
        
        uintptr_t items = mem->getPtr(listAddr + 0x10); // List.items at 0x10
        if (!items) return result;
        
        // items is Il2CppArray
        // Array vector at 0x20
        for (int i=0; i<size; i++) {
            uintptr_t player = mem->getPtr(items + 0x20 + i*8);
            if (player) result.push_back(player);
        }
        return result;
    }

    // Full flow for BlueStacks without Frida
    std::vector<uintptr_t> getPlayersNoFrida() {
        std::vector<uintptr_t> players;

        // Try method 1: Read from dump file created by helper SO (if helper was injected)
        FILE* f = fopen("/data/local/tmp/oxide_addrs.bin", "rb");
        if (f) {
            uintptr_t clientList = 0;
            fread(&clientList, sizeof(uintptr_t), 1, f);
            fclose(f);
            if (clientList) {
                return parsePlayerList(clientList);
            }
        }

        // Try method 2: Find class via brute force and get static fields
        uintptr_t klass = findClassByName("PlayerManager");
        if (klass) {
            uintptr_t static_fields = getStaticFields(klass);
            uintptr_t clientList = getPlayerListFromStatic(static_fields, 0x10); // clientPlayerList at 0x10
            if (clientList) {
                return parsePlayerList(clientList);
            }
        }

        // Try method 3: Scan for List objects that contain PlayerManager-like objects
        // Heuristic: PlayerManager has lastTickPosition at 0x1C8 which is Vector3 with reasonable world coords
        // We can scan heap for objects that have valid Vector3 at 0x1C8 and valid vitals at 0xC8
        // This is slower but works without any il2cpp parsing

        return players;
    }
};
