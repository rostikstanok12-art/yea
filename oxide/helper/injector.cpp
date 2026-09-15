#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <dlfcn.h>
#include <dirent.h>
#include <sys/uio.h>
#include <elf.h>
#include <android/log.h>

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, "Injector", __VA_ARGS__)

// Simplified ptrace injector for BlueStacks - injects liboxide_helper.so into Oxide process
// Works with root (adb root -> uid 0) and selinux permissive

uintptr_t get_module_base(pid_t pid, const char* module_name) {
    char filename[64];
    char line[1024];
    snprintf(filename, sizeof(filename), "/proc/%d/maps", pid);
    FILE* fp = fopen(filename, "r");
    if (!fp) return 0;
    uintptr_t addr = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, module_name)) {
            char* pch = strtok(line, "-");
            addr = strtoul(pch, NULL, 16);
            break;
        }
    }
    fclose(fp);
    return addr;
}

uintptr_t get_remote_addr(pid_t target_pid, const char* module_name, void* local_addr) {
    uintptr_t local_base = get_module_base(getpid(), module_name);
    uintptr_t remote_base = get_module_base(target_pid, module_name);
    uintptr_t offset = (uintptr_t)local_addr - local_base;
    return remote_base + offset;
}

// Find PID by package name
int get_pid_by_name(const char* pkg) {
    DIR* dir = opendir("/proc");
    if (!dir) return -1;
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        int pid = atoi(entry->d_name);
        if (pid <= 0) continue;
        char cmdline_path[64];
        snprintf(cmdline_path, sizeof(cmdline_path), "/proc/%d/cmdline", pid);
        FILE* fp = fopen(cmdline_path, "r");
        if (!fp) continue;
        char cmdline[256] = {0};
        fgets(cmdline, sizeof(cmdline), fp);
        fclose(fp);
        if (strstr(cmdline, pkg)) {
            closedir(dir);
            return pid;
        }
    }
    closedir(dir);
    return -1;
}

// ptrace injection using dlopen
bool inject_so(pid_t pid, const char* so_path) {
    LOGD("Injecting %s into pid %d", so_path, pid);

    // Attach
    if (ptrace(PTRACE_ATTACH, pid, NULL, NULL) == -1) {
        LOGD("PTRACE_ATTACH failed");
        return false;
    }
    waitpid(pid, NULL, 0);

    // Get remote dlopen addr
    void* local_dlopen = dlsym(RTLD_DEFAULT, "dlopen");
    uintptr_t remote_dlopen = get_remote_addr(pid, "libc.so", local_dlopen);
    if (!remote_dlopen) {
        remote_dlopen = get_remote_addr(pid, "libdl.so", local_dlopen);
    }
    LOGD("remote dlopen: %lx local %p", remote_dlopen, local_dlopen);

    // Allocate memory in remote process for so_path
    // Use mmap via remote call - simplified: we use process_vm_writev to write to remote stack?
    // For BlueStacks, easier method: use /proc/pid/mem with ptrace

    // For simplicity in BlueStacks with root, we can use a different method:
    // Write so_path to /data/local/tmp and then use dlopen with that path via remote call
    // We will use ptrace to call mmap, then write, then dlopen

    // This is simplified injector - for production use, need full shellcode
    // For BlueStacks, we can try using __android_log_print to call dlopen via injected thread

    // Alternative simple method for BlueStacks with root: 
    // Use setprop wrap.<pkg> to preload SO on next launch
    // But we want live injection, so we use ptrace + shellcode

    // Detach for now - implement full injection later
    ptrace(PTRACE_DETACH, pid, NULL, NULL);
    
    LOGD("Injection placeholder - use setprop method for BlueStacks");
    return true;
}

// For BlueStacks, easiest injection without Frida is via setprop wrap
bool inject_via_wrap(const char* pkg, const char* so_path) {
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "setprop wrap.%s LD_PRELOAD=%s", pkg, so_path);
    LOGD("Running: %s", cmd);
    int ret = system(cmd);
    LOGD("setprop ret %d", ret);
    // Need to restart app
    snprintf(cmd, sizeof(cmd), "am force-stop %s", pkg);
    system(cmd);
    sleep(1);
    snprintf(cmd, sizeof(cmd), "am start -n %s/com.unity3d.player.UnityPlayerActivity", pkg);
    system(cmd);
    return true;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Usage: %s <package_name> <so_path>\n", argv[0]);
        printf("Example: %s com.oxide.survivalisland /data/local/tmp/liboxide_helper.so\n", argv[0]);
        printf("BlueStacks: adb -s 127.0.0.1:5555 shell su -c \"/data/local/tmp/injector com.oxide.survivalisland /data/local/tmp/liboxide_helper.so\"\n");
        return 1;
    }

    const char* pkg = argv[1];
    const char* so_path = argv[2];

    int pid = get_pid_by_name(pkg);
    if (pid == -1) {
        LOGD("PID not found for %s, trying wrap method", pkg);
        inject_via_wrap(pkg, so_path);
        return 0;
    }

    LOGD("Found PID %d for %s", pid, pkg);

    // Try ptrace injection
    if (!inject_so(pid, so_path)) {
        LOGD("ptrace inject failed, trying wrap");
        inject_via_wrap(pkg, so_path);
    }

    // Check if dump file created
    sleep(3);
    FILE* f = fopen("/data/local/tmp/oxide_dump.txt", "r");
    if (f) {
        char buf[1024];
        while (fgets(buf, sizeof(buf), f)) {
            printf("%s", buf);
        }
        fclose(f);
        LOGD("Dump file exists, injection success");
    } else {
        LOGD("Dump file not found, helper may not have loaded yet");
    }

    return 0;
}
