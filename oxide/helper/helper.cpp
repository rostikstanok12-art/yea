#include <dlfcn.h>
#include <unistd.h>
#include <fcntl.h>
#include <android/log.h>
#include <cstdio>
#include <cstring>
#include <thread>

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, "OxideHelper", __VA_ARGS__)

// Il2Cpp API types
typedef void* (*il2cpp_domain_get_t)();
typedef void* (*il2cpp_thread_attach_t)(void* domain);
typedef void* (*il2cpp_domain_assembly_open_t)(void* domain, const char* name);
typedef void* (*il2cpp_assembly_get_image_t)(void* assembly);
typedef void* (*il2cpp_class_from_name_t)(void* image, const char* namespaze, const char* name);
typedef void* (*il2cpp_class_get_field_from_name_t)(void* klass, const char* name);
typedef void (*il2cpp_field_static_get_value_t)(void* field, void* value);
typedef void* (*il2cpp_class_get_type_t)(void* klass);

static void do_work() {
    LOGD("Oxide helper loaded, trying to get PlayerManager list");

    // Try to dlopen libil2cpp.so from current process (we are inside game)
    void* il2cpp_handle = dlopen("libil2cpp.so", RTLD_NOW);
    if (!il2cpp_handle) {
        LOGD("dlopen libil2cpp.so failed: %s", dlerror());
        // Try full path
        il2cpp_handle = dlopen("/data/app/com.oxide.survivalisland-1/lib/arm64/libil2cpp.so", RTLD_NOW);
        if (!il2cpp_handle) {
            LOGD("second dlopen failed");
            return;
        }
    }

    auto il2cpp_domain_get = (il2cpp_domain_get_t)dlsym(il2cpp_handle, "il2cpp_domain_get");
    auto il2cpp_thread_attach = (il2cpp_thread_attach_t)dlsym(il2cpp_handle, "il2cpp_thread_attach");
    auto il2cpp_domain_assembly_open = (il2cpp_domain_assembly_open_t)dlsym(il2cpp_handle, "il2cpp_domain_assembly_open");
    auto il2cpp_assembly_get_image = (il2cpp_assembly_get_image_t)dlsym(il2cpp_handle, "il2cpp_assembly_get_image");
    auto il2cpp_class_from_name = (il2cpp_class_from_name_t)dlsym(il2cpp_handle, "il2cpp_class_from_name");
    auto il2cpp_class_get_field_from_name = (il2cpp_class_get_field_from_name_t)dlsym(il2cpp_handle, "il2cpp_class_get_field_from_name");
    auto il2cpp_field_static_get_value = (il2cpp_field_static_get_value_t)dlsym(il2cpp_handle, "il2cpp_field_static_get_value");

    if (!il2cpp_domain_get || !il2cpp_thread_attach || !il2cpp_domain_assembly_open) {
        LOGD("Failed to get il2cpp exports");
        return;
    }

    // Attach thread
    void* domain = il2cpp_domain_get();
    if (!domain) {
        LOGD("domain null");
        return;
    }
    il2cpp_thread_attach(domain);
    LOGD("domain: %p", domain);

    // Open Assembly-CSharp
    void* assembly = il2cpp_domain_assembly_open(domain, "Assembly-CSharp");
    if (!assembly) {
        LOGD("Assembly-CSharp not found, trying Assembly-CSharp.dll");
        assembly = il2cpp_domain_assembly_open(domain, "Assembly-CSharp.dll");
    }
    if (!assembly) {
        LOGD("assembly null");
        return;
    }
    void* image = il2cpp_assembly_get_image(assembly);
    LOGD("image: %p", image);

    // Get PlayerManager class - global namespace ""
    void* playerMgrClass = il2cpp_class_from_name(image, "", "PlayerManager");
    if (!playerMgrClass) {
        LOGD("PlayerManager class not found, trying with namespace");
        playerMgrClass = il2cpp_class_from_name(image, "Oxide", "PlayerManager");
    }
    if (!playerMgrClass) {
        LOGD("PlayerManager still not found");
        return;
    }
    LOGD("PlayerManager class: %p", playerMgrClass);

    // Get static fields
    void* clientListField = il2cpp_class_get_field_from_name(playerMgrClass, "clientPlayerList");
    void* activeListField = il2cpp_class_get_field_from_name(playerMgrClass, "activePlayerList");
    void* sleepingListField = il2cpp_class_get_field_from_name(playerMgrClass, "sleepingPlayerList");

    LOGD("fields: client %p active %p sleeping %p", clientListField, activeListField, sleepingListField);

    // Get values
    void* clientList = nullptr;
    void* activeList = nullptr;
    void* sleepingList = nullptr;

    if (clientListField) {
        il2cpp_field_static_get_value(clientListField, &clientList);
        LOGD("clientPlayerList: %p", clientList);
    }
    if (activeListField) {
        il2cpp_field_static_get_value(activeListField, &activeList);
        LOGD("activePlayerList: %p", activeList);
    }
    if (sleepingListField) {
        il2cpp_field_static_get_value(sleepingListField, &sleepingList);
        LOGD("sleepingPlayerList: %p", sleepingList);
    }

    // Write to file for external ESP to read
    // BlueStacks: /data/local/tmp is world writable with root
    const char* outPath = "/data/local/tmp/oxide_dump.txt";
    FILE* f = fopen(outPath, "w");
    if (f) {
        fprintf(f, "clientPlayerList=%p\n", clientList);
        fprintf(f, "activePlayerList=%p\n", activeList);
        fprintf(f, "sleepingPlayerList=%p\n", sleepingList);
        fprintf(f, "PlayerManagerClass=%p\n", playerMgrClass);
        fprintf(f, "domain=%p\n", domain);
        fclose(f);
        LOGD("Wrote to %s", outPath);
        // chmod 777
        chmod(outPath, 0777);
    } else {
        LOGD("Failed to open %s", outPath);
    }

    // Also write binary for fast reading
    const char* binPath = "/data/local/tmp/oxide_addrs.bin";
    FILE* fb = fopen(binPath, "wb");
    if (fb) {
        fwrite(&clientList, sizeof(void*), 1, fb);
        fwrite(&activeList, sizeof(void*), 1, fb);
        fwrite(&sleepingList, sizeof(void*), 1, fb);
        fclose(fb);
        chmod(binPath, 0777);
    }

    LOGD("Helper done");
}

__attribute__((constructor))
void init() {
    LOGD("Oxide helper constructor called");
    // Run in separate thread to not block dlopen
    std::thread t([](){
        sleep(2); // wait for game to init il2cpp
        for (int i=0; i<10; i++) {
            do_work();
            sleep(5);
        }
    });
    t.detach();
}
