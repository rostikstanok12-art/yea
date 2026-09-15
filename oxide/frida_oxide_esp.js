// Oxide: Survival Island - Frida ESP & Weapon Mods
// Uses il2cpp_dump_and_api_summary.txt offsets
// Target: libil2cpp.so on emulator (x86_64 or arm64)
// Emulator OpenGL - works with LDPlayer, MuMu, etc.

const base = Module.findBaseAddress("libil2cpp.so");
if (!base) {
    console.log("[!] libil2cpp.so not found");
} else {
    console.log("[+] libil2cpp.so base: " + base);

    // Example RVAs from dump - adjust to your version
    // PlayerManager activePlayerList is static field
    // We can get it via il2cpp API

    const il2cpp_domain_get = new NativeFunction(Module.findExportByName("libil2cpp.so", "il2cpp_domain_get"), "pointer", []);
    const il2cpp_domain_assembly_open = new NativeFunction(Module.findExportByName("libil2cpp.so", "il2cpp_domain_assembly_open"), "pointer", ["pointer", "pointer"]);
    const il2cpp_assembly_get_image = new NativeFunction(Module.findExportByName("libil2cpp.so", "il2cpp_assembly_get_image"), "pointer", ["pointer"]);
    const il2cpp_class_from_name = new NativeFunction(Module.findExportByName("libil2cpp.so", "il2cpp_class_from_name"), "pointer", ["pointer", "pointer", "pointer"]);
    const il2cpp_class_get_field_from_name = new NativeFunction(Module.findExportByName("libil2cpp.so", "il2cpp_class_get_field_from_name"), "pointer", ["pointer", "pointer"]);
    const il2cpp_field_static_get_value = new NativeFunction(Module.findExportByName("libil2cpp.so", "il2cpp_field_static_get_value"), "void", ["pointer", "pointer"]);

    function getClass(image, ns, name) {
        return il2cpp_class_from_name(image, Memory.allocUtf8String(ns), Memory.allocUtf8String(name));
    }

    function dumpPlayers() {
        const domain = il2cpp_domain_get();
        const assembly = il2cpp_domain_assembly_open(domain, Memory.allocUtf8String("Assembly-CSharp"));
        const image = il2cpp_assembly_get_image(assembly);
        const playerMgrClass = getClass(image, "", "PlayerManager");
        console.log("[+] PlayerManager class: " + playerMgrClass);

        const field = il2cpp_class_get_field_from_name(playerMgrClass, Memory.allocUtf8String("clientPlayerList"));
        console.log("[+] clientPlayerList field: " + field);

        const listPtr = Memory.alloc(8);
        il2cpp_field_static_get_value(field, listPtr);
        const list = listPtr.readPointer();
        console.log("[+] clientPlayerList instance: " + list);

        if (list.isNull()) {
            console.log("[!] list is null, try activePlayerList");
            const field2 = il2cpp_class_get_field_from_name(playerMgrClass, Memory.allocUtf8String("activePlayerList"));
            il2cpp_field_static_get_value(field2, listPtr);
            const list2 = listPtr.readPointer();
            console.log("[+] activePlayerList: " + list2);
        }
    }

    // Hook weapon config for no recoil / no spread
    // HitscanWeaponConfig - from dump, damage at 0x18, etc
    // Example: hook get_damage or set recoil

    // No Recoil hook - RecoilConfig fields
    // We can hook PlayerWeapon or HitscanWeaponConfig methods
    // For emulator, we can directly patch memory

    // Example: patch recoil
    setTimeout(dumpPlayers, 2000);

    // Weapon mods: find HitscanWeaponConfig and zero recoil
    // Using static field scanning or via PlayerManager equipment
}

// OpenGL ESP - for emulator we can hook Unity Camera
const unityBase = Module.findBaseAddress("libunity.so");
if (unityBase) {
    console.log("[+] libunity.so base: " + unityBase);
    // Camera::GetViewMatrix etc could be hooked here for W2S
}

// For LDPlayer OpenGL, we can also hook eglSwapBuffers to draw ImGui externally
// This script is for analysis, final ESP is in native libmain.so
