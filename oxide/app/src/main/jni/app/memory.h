#pragma once
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/syscall.h>
#include <sys/uio.h>
#include <string>
#include <vector>
#include <dlfcn.h>

// Reuse original panda memory class but renamed
class Memory {
public:
    int process_vm_readv_syscall = 270;
    int process_vm_writev_syscall = 271;
    int pid = -1;

    void Init(int pid1) {
        pid = pid1;
        process_vm_readv_syscall = 270;
        process_vm_writev_syscall = 271;
    }

    int getPID(const char* packageName) {
        int id = -1;
        DIR* dir;
        FILE* fp;
        char filename[64];
        char cmdline[64];
        struct dirent* entry;
        dir = opendir("/proc");
        while ((entry = readdir(dir)) != NULL) {
            id = atoi(entry->d_name);
            if (id != 0) {
                sprintf(filename, "/proc/%d/cmdline", id);
                fp = fopen(filename, "r");
                if (fp) {
                    fgets(cmdline, sizeof(cmdline), fp);
                    fclose(fp);
                    if (strcmp(packageName, cmdline) == 0) {
                        closedir(dir);
                        return id;
                    }
                }
            }
        }
        closedir(dir);
        return -1;
    }

    ssize_t process_v(pid_t __pid, const struct iovec* __local_iov, unsigned long __local_iov_count,
                      const struct iovec* __remote_iov, unsigned long __remote_iov_count,
                      unsigned long __flags, bool iswrite) {
        return syscall((iswrite ? process_vm_writev_syscall : process_vm_readv_syscall),
                       __pid, __local_iov, __local_iov_count, __remote_iov, __remote_iov_count, __flags);
    }

    bool pvm(void* address, void* buffer, size_t size, bool iswrite) {
        struct iovec local[1];
        struct iovec remote[1];
        local[0].iov_base = buffer;
        local[0].iov_len = size;
        remote[0].iov_base = address;
        remote[0].iov_len = size;
        if (pid < 0) return false;
        ssize_t bytes = process_v(pid, local, 1, remote, 1, 0, iswrite);
        return bytes == (ssize_t)size;
    }

    bool readv(uintptr_t address, void* buffer, size_t size) {
        return pvm(reinterpret_cast<void*>(address), buffer, size, false);
    }
    bool writev(uintptr_t address, void* buffer, size_t size) {
        return pvm(reinterpret_cast<void*>(address), buffer, size, true);
    }

    float getFloat(uintptr_t addr) { float v=0; readv(addr,&v,4); return v; }
    int getInt(uintptr_t addr) { int v=0; readv(addr,&v,4); return v; }
    uintptr_t getPtr(uintptr_t addr) { uintptr_t v=0; readv(addr,&v,8); return v; }
    uintptr_t getPtr32(uintptr_t addr) { uint32_t v=0; readv(addr,&v,4); return v; }
    void writeFloat(uintptr_t addr, float data) { writev(addr,&data,4); }
    void writeInt(uintptr_t addr, int data) { writev(addr,&data,4); }

    uintptr_t get_module_base(const char* module_name) {
        FILE* fp;
        uintptr_t addr = 0;
        char* pch;
        char filename[64];
        char line[1024];
        snprintf(filename, sizeof(filename), "/proc/%d/maps", pid);
        fp = fopen(filename, "r");
        if (fp != NULL) {
            while (fgets(line, sizeof(line), fp)) {
                if (strstr(line, module_name)) {
                    pch = strtok(line, "-");
                    addr = strtoul(pch, NULL, 16);
                    if (addr == 0x8000) addr = 0;
                    break;
                }
            }
            fclose(fp);
        }
        return addr;
    }

    // Get il2cpp base and unity base for emulator
    uintptr_t get_il2cpp_base() { return get_module_base("libil2cpp.so"); }
    uintptr_t get_unity_base() { return get_module_base("libunity.so"); }
    uintptr_t get_libmain_base() { return get_module_base("libmain.so"); }

    // Read Unity string (Il2CppString)
    std::string getUnityString(uintptr_t strAddr) {
        if (!strAddr) return "";
        int len = getInt(strAddr + 0x10);
        if (len <=0 || len > 100) return "";
        std::vector<uint16_t> buf(len+1);
        readv(strAddr + 0x14, buf.data(), len*2);
        std::string out;
        for (int i=0;i<len;i++) {
            if (buf[i] < 0x80) out.push_back((char)buf[i]);
            else break;
        }
        return out;
    }

    // Read List<T> size and items
    int getListSize(uintptr_t listAddr) {
        if (!listAddr) return 0;
        return getInt(listAddr + 0x18);
    }
    uintptr_t getListItems(uintptr_t listAddr) {
        if (!listAddr) return 0;
        return getPtr(listAddr + 0x10);
    }
    uintptr_t getArrayElement(uintptr_t arrayAddr, int index) {
        uintptr_t items = getPtr(arrayAddr + 0x20); // for Il2CppArray
        // Actually for List, items is array
        // items + 0x20 + index*8
        return getPtr(items + 0x20 + index*8);
    }
};
