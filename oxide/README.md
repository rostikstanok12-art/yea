# Oxide: Survival Island - Panda Plugin Adapted for Emulator OpenGL

## Что это
Адаптация исходного Panda плагина (китайский сурс с ImGui + EGL + process_vm_readv) под **Oxide: Survival Island** для работы на **эмуляторе** (LDPlayer, MuMu, Bluestacks) с **OpenGL**.

## Структура проекта
```
oxide/
├── app/
│   ├── src/main/jni/
│   │   ├── main.cpp - EGL OpenGL init, ImGui loop, JNI bridge
│   │   ├── Android.mk / Application.mk - сборка под arm64-v8a + x86_64 (эмулятор)
│   │   ├── app/
│   │   │   ├── config.h - конфиг меню Oxide (ESP, Aim, Weapon, World)
│   │   │   ├── oxide_offsets.h - оффсеты из il2cpp_dump_and_api_summary.txt
│   │   │   ├── oxide_sdk.h - структуры PlayerManager, WeaponConfig, W2S
│   │   │   ├── oxide_esp.h/cpp - логика ESP/Aim/NoRecoil для Oxide
│   │   │   ├── memory.h - чтение памяти через process_vm_readv (для эмулятора)
│   │   │   ├── layout.cpp - ImGui меню (ESP / AIM / WEAPON / WORLD)
│   │   │   └── Vector3.hpp - векторная математика
│   │   └── imgui/ - ImGui + backends OpenGL3 + Android
│   └── src/main/java/com/KITE/ - Java оверлей, получение PID Oxide
├── frida_oxide_esp.js - Frida скрипт для поиска clientPlayerList и теста оффсетов
├── il2cpp_dump_and_api_summary.txt - дамп (скопируй из корня)
└── build.gradle / settings.gradle
```

## Оффсеты из дампа
Из `il2cpp_dump_and_api_summary.txt`:

**PlayerManager (TypeDefIndex 9223)**
- `worldCameraRoot 0x68`, `mouseLook 0x70`, `vitals 0xC8`, `characterModel 0x150`
- Static: `sleepingPlayerList 0x0`, `activePlayerList 0x8`, `clientPlayerList 0x10`

**MouseLook**
- `XSensitivity 0x10`, `YSensitivity 0x14`, `m_CharacterTargetRot 0x30`

**HitscanWeaponConfig**
- `damage 0x18`, `MaxDistance 0x2C`, `BulletSpeed 0x30`, `fireRate 0x34`, `BaseDispersion 0x3C`, `recoilConfig 0x68`, `dispersionConfig 0x60`

**RecoilConfig**
- `MinRecoil 0x18`, `MaxRecoil 0x20`, `RecoilAimMultiplier 0x30`

**DispersionConfig**
- `minDispersion 0x20`, `maxDispersion 0x24`

Полный список в `SECTION 4: QUICK-LOOKUP FIELD OFFSET TABLE` дампа.

## Как работает на эмуляторе OpenGL

1. **Java часть** (`MainActivity.java`):
   - Запрашивает `SYSTEM_ALERT_WINDOW`
   - Ищет PID Oxide через `/proc` (`com.oxide.survival` или `com.oxide.survivalisland`)
   - Передает PID в натив через `SuperJNI.setPid(pid)`
   - Показывает `FloatTool` - SurfaceView на весь экран с прозрачным EGL

2. **Натив часть** (`main.cpp`):
   - `setSurface` - получает `ANativeWindow`, создает EGL контекст OpenGL ES 3.0 (`eglCreateContext` с `CLIENT_VERSION 3`)
   - `initGui` - инициализирует ImGui, создает поток 60fps
   - В лупе: `Update()` читает память игры, `Render()` рисует ESP через `ImGui::GetForegroundDrawList()`

3. **Чтение памяти** (`memory.h`):
   - Использует `process_vm_readv` syscall 270/271 (как в оригинальном Panda)
   - `get_module_base("libil2cpp.so")` парсит `/proc/pid/maps` для базы il2cpp
   - Для эмулятора x86_64 и arm64-v8a работает одинаково, т.к. syscall одинаковый на Android x86

4. **ESP Логика** (`oxide_esp.cpp`):
   - `getPlayerList()` - должен парсить `Il2CppClass->static_fields` для `clientPlayerList`
   - Сейчас placeholder, нужно дописать парсинг Il2Cpp metadata (домен, ассембли, класс)
   - `getPlayerPos()` читает `lastTickPosition` at `0x1C8`
   - `WorldToScreen()` - пока заглушка, нужно читать viewMatrix из Camera.main
   - Для эмулятора OpenGL viewMatrix можно получить хуком `libunity.so` или чтением CameraManager

5. **Меню** (`layout.cpp`):
   - 4 вкладки: ESP, AIM, WEAPON, WORLD
   - Слайдеры для FOV, дистанции, радара
   - Кнопки для поиска PID, применения weapon mods

## Что нужно доделать для полной работы

1. **Парсинг static fields**:
   ```cpp
   // Получить Il2CppClass для PlayerManager через il2cpp API или сканированием памяти
   // static_fields = class->static_fields
   // clientPlayerList = static_fields + 0x10
   // Затем: List size at 0x18, items at 0x10
   ```

2. **ViewMatrix**:
   - Найти Camera.main через il2cpp
   - Или хукать Unity: `Camera.get_worldToCameraMatrix`
   - Для эмулятора можно читать из `libunity.so` + оффсет CameraManager

3. **Player name / health**:
   - Health: `PlayerManager 0xC8 vitals -> GenericVitals 0x88 maxHealth` + текущее здоровье в zeT
   - Name: читать `nicklabel` или `DisplayName` at 0x88

4. **No Recoil / No Spread**:
   - Получить `PlayerManager 0xE8 equipment -> weaponReference -> PlayerWeapon -> weaponConfig`
   - Затем `recoilConfig 0x68` занулить

## Сборка

В Android Studio:
- Открыть папку `oxide/`
- Синхронизировать Gradle
- NDK должен быть установлен
- Собрать APK, дать рут права на эмуляторе (`adb root`, `su`)

Для LDPlayer:
- Включить root в настройках эмулятора
- Установить APK Panda
- Запустить Oxide, затем запустить оверлей

## Frida для тестов

Используй `frida_oxide_esp.js` для проверки оффсетов без пересборки:

```bash
frida -U -f com.oxide.survival -l frida_oxide_esp.js --no-pause
```

Скрипт выведет адреса `clientPlayerList` и `activePlayerList`.

## Отличия от оригинала Panda

- Оригинал был под PUBG-like с матрицей из libUE4
- Здесь адаптировано под Unity IL2CPP (Oxide)
- Добавлена поддержка x86_64 для эмулятора (Application.mk: `APP_ABI := arm64-v8a x86_64`)
- OpenGL ES 3.0 вместо ES 2.0 для лучшей совместимости с эмуляторами
- Убран китайский логин `wy.llua.cn`, упрощен Java

## Дальше

- Могу дописать полный парсер Il2CppClass для внешнего чтения
- Могу сделать internal версию (инжект libmain.so прямо в процесс Oxide через `ptrace` или `zygisk`)
- Могу добавить скелетон ESP через чтение Animator bones

Скажи что делаем дальше - дописываем W2S, парсер листа, или сразу собираем тестовый APK.
