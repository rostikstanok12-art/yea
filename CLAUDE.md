# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this repository is

Not a buildable project. It is a two-file reference bundle for reverse-engineering the Unity IL2CPP mobile game **Oxide: Survival Island** (Rust-inspired, Mirror networking, ARM64/x86_64):

- `il2cpp_dump_and_api_summary.txt` — hand-curated cheat sheet distilled from Il2CppDumper outputs (`dump.cs`, `il2cpp-api.h`, `script.json`, `stringliteral.json`). ~2800 lines, four sections (grep for `^SECTION` to jump):
  1. Overview & RVA math (`VA = base(libil2cpp.so) + RVA`).
  2. IL2CPP C-API catalog (~240 functions grouped by category: init, GC, domain/assembly, class, type, field, method, object, string, array, exception, threading, profiler) plus Frida/C++ hook patterns.
  3. Core gameplay classes from `Assembly-CSharp.dll` — `PlayerManager`, `CharacterMovement`, `MouseLook`, `PlayerWeapon`, `HitscanWeaponConfig`, `RecoilConfig`, `DispersionConfig`, `PlayerVitals`/`EntityVitals`/`GenericVitals`/`zeT`, `HelicopterController`, `BuildingGrade`, `AutoTurret`, airdrop/sleeper logic.
  4. Quick-lookup field offset table.
- `熊猫插件(сурс Китай Чита).zip` — a Gradle/AIDE Android project that *consumes* the information in the text file: a mod-menu host for the game. Not extracted into the repo tree.

`README.md` is a placeholder (`# yea`). There is no root build system, no tests, no linter, no package manifest — do not invent build/test commands.

## Working with the dump reference

- Treat the text file as the source of truth; prefer `Grep` over reading the whole thing. Useful anchors: `il2cpp_` for API lookups, class names like `PlayerWeapon` or `PlayerVitals` for Section 3 entries, `RVA` or `Offset` for addresses, `[CATEGORY]` for API groupings.
- RVAs and field offsets in the file are tied to the specific build of the game that was dumped; treat them as examples, not stable ABI. Section 1 states the formula for converting them to runtime addresses via Frida.

## Working with the plugin archive

- Inspect contents without unpacking into the repo: `unzip -l "熊猫插件(сурс Китай Чита).zip"`. Extract to the scratchpad (`/tmp/claude-0/.../scratchpad`), never into the working tree — it is large (~15 MB, includes `.dex`, `.jar`, `app/build/` output) and would clutter git.
- Layout inside the archive: standard Android Gradle project — `app/build.gradle`, `app/src/main/AndroidManifest.xml`, Java under `app/src/main/java/com/KITE/` (`MainActivity`, `SuperJNI`, floating-menu tools, RC4 util), AIDL at `app/src/main/aidl/com/KITE/IPCCond/IMutual.aidl`, native code under `app/src/main/jni/` built with `Android.mk`/`Application.mk` (ndk-build), bundled payload at `app/src/main/assets/main.jar`. Some directory names are Chinese (CP437-mojibake in the listing).
- The archive carries pre-built `app/build/bin/*.dex` and intermediate class files — do not treat those as sources.

## Git / branch convention for this session

- Session-designated branch: `claude/blissful-bell-ppvyxt`. Commit and push development there; do not push to `main` without explicit instruction.
- Remote: `origin` → `rostikstanok12-art/yea` on GitHub.
