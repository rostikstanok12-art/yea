# Build for BlueStacks 5 - Oxide Panda

## Requirements
- Android Studio with NDK
- BlueStacks 5 with Root enabled
- ADB from Android SDK

## Steps

1. Enable root in BlueStacks:
   - BlueStacks Settings -> Advanced -> ADB ON, Root ON
   - Or edit C:\ProgramData\BlueStacks_nxt\bluestacks.conf: bst.feature.rooting=1

2. Connect ADB and root:
```bat
adb connect 127.0.0.1:5555
adb -s 127.0.0.1:5555 root
adb -s 127.0.0.1:5555 shell id
# uid=0(root)
adb -s 127.0.0.1:5555 shell setenforce 0
adb -s 127.0.0.1:5555 shell getenforce
# Permissive
```

3. Build APK:
   - Open oxide/ in Android Studio
   - Sync Gradle
   - Build -> Build APK
   - Output: oxide/app/build/outputs/apk/debug/app-debug.apk

4. Install to BlueStacks:
```bat
adb -s 127.0.0.1:5555 install -r app-debug.apk
adb -s 127.0.0.1:5555 shell am start -n com.KITE.OxideEmu/.MainActivity
```

5. In BlueStacks:
   - Launch Oxide first
   - Then launch Oxide Panda app
   - Click "Check Root" - should show uid=0 and Permissive
   - Click "Start ESP Overlay" - overlay should appear over game
   - If no players, check PID and libil2cpp base in status

## Debugging on BlueStacks

```bat
adb -s 127.0.0.1:5555 logcat -s OxideMain:D BlueStacks:D OxideESP:D OxideBS:D
```

Check:
- il2cpp base found?
- pid found?
- players found?

If players = 0, need to implement static fields parsing:
- Use Frida script: frida -U -f com.oxide.survivalisland -l frida_oxide_esp.js

## Offsets already applied

All offsets from il2cpp_dump_and_api_summary.txt are in:
- oxide_offsets.h
- oxide_offsets_bluestacks.h
- OFFSETS_BLUESTACKS.txt

No need to manually set, just build.

## BlueStacks OpenGL

- Uses EGL OpenGL ES 3.0 (fallback to 2.0)
- ImGui renders via OpenGL3 backend
- Works with BlueStacks 5 OpenGL renderer (Settings -> Graphics -> Renderer -> OpenGL)
- For best performance, set BlueStacks to High Performance, 4 cores, 4GB RAM
