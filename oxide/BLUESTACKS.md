# BlueStacks 5 - Oxide Panda Root via ADB

## Как в твоем примере - рабочий флоу для BlueStacks с встроенным рутом

### 1. Подключение

```powershell
& "C:\Users\Rostic43\AppData\Local\Android\Sdk\platform-tools\adb.exe" connect 127.0.0.1:5555
& "C:\Users\Rostic43\AppData\Local\Android\Sdk\platform-tools\adb.exe" connect 127.0.0.1:5565
& "C:\Users\Rostic43\AppData\Local\Android\Sdk\platform-tools\adb.exe" connect 127.0.0.1:7555
& "C:\Users\Rostic43\AppData\Local\Android\Sdk\platform-tools\adb.exe" connect 127.0.0.1:16416
```

BlueStacks 5 порты:
- `127.0.0.1:5555` - основной инстанс
- `127.0.0.1:5565` - второй
- `127.0.0.1:5575` - третий
- `127.0.0.1:5585` - четвертый

MuMu 12:
- `127.0.0.1:7555`
- `127.0.0.1:16416`

### 2. Поднять adbd до root (BlueStacks 5 и MuMu 12 поддерживают из коробки)

```powershell
& adb -s 127.0.0.1:5555 root
# ответ: "restarting adbd as root"

& adb -s 127.0.0.1:7555 root
```

В BlueStacks 5 нужно включить в настройках:
- Settings -> Advanced -> Android Debug Bridge (ADB) -> ON
- Settings -> Advanced -> Root access -> ON (в некоторых версиях через bs_config)

### 3. Проверка

```powershell
& adb -s 127.0.0.1:5555 shell id
# uid=0(root) gid=0(root) groups=0(root),1004(input)...

& adb -s 127.0.0.1:5555 shell getenforce
# Permissive (или Enforcing - тогда делаем setenforce 0)
```

Если Enforcing:
```powershell
& adb -s 127.0.0.1:5555 shell setenforce 0
& adb -s 127.0.0.1:5555 shell getenforce
# Permissive
```

### 4. Проверка Oxide пакета

```powershell
& adb -s 127.0.0.1:5555 shell pm list packages | findstr oxide
# com.oxide.survivalisland
# com.oxide.survival

& adb -s 127.0.0.1:5555 shell ps -A | findstr oxide
& adb -s 127.0.0.1:5555 shell pidof com.oxide.survivalisland
```

### 5. Проверка libil2cpp.so базы (для оффсетов)

```powershell
& adb -s 127.0.0.1:5555 shell "cat /proc/$(pidof com.oxide.survivalisland)/maps | grep libil2cpp"
# 7a3b...-7a3c... r-xp ... libil2cpp.so
# База = первый адрес, например 0x7a3b200000

& adb -s 127.0.0.1:5555 shell "cat /proc/$(pidof com.oxide.survivalisland)/maps | grep libunity"
& adb -s 127.0.0.1:5555 shell "cat /proc/$(pidof com.oxide.survivalisland)/maps | grep libmain"
```

### 6. Установка Panda overlay

```powershell
& adb -s 127.0.0.1:5555 install -r oxide_panda.apk
& adb -s 127.0.0.1:5555 shell am start -n com.KITE.OxideEmu/.MainActivity
```

Или вручную в BlueStacks установить APK.

### 7. Как работает рут в Panda на BlueStacks

Java часть (`MainActivity.java`) теперь:
- Ищет PID через `/proc` (работает только с root)
- `SuperJNI.setPid(pid)` передает PID в натив
- Натив `Memory` класс использует `process_vm_readv` syscall 270/271
- Для этого нужен `uid=0` и `Permissive` - как раз что дает `adb root` + `setenforce 0`

Если без рута, то `/proc/pid/maps` не читается и `process_vm_readv` падает с `EPERM`.

### 8. Оффсеты для BlueStacks

Все оффсеты уже подставлены в:
- `oxide/app/src/main/jni/app/oxide_offsets.h` - базовые
- `oxide/app/src/main/jni/app/oxide_offsets_bluestacks.h` - расширенные с комментами для BlueStacks

Основные для BlueStacks x86_64 / arm64:
```
PlayerManager:
  lastTickPosition 0x1C8 (Vector3)
  vitals 0xC8
  worldCameraRoot 0x68
  Static clientPlayerList 0x10 (List<PlayerManager>)

List:
  items 0x10
  size 0x18
Array:
  vector 0x20

HitscanWeaponConfig:
  damage 0x18
  BulletSpeed 0x30
  recoilConfig 0x68
  dispersionConfig 0x60

RecoilConfig:
  MinRecoil 0x18
  MaxRecoil 0x20
  RecoilAimMultiplier 0x30
```

### 9. Быстрый запуск через скрипты

Windows:
```
.\oxide\scripts\bluestacks_adb_root.bat
```

Linux/macOS:
```
chmod +x oxide/scripts/bluestacks_adb_root.sh
./oxide/scripts/bluestacks_adb_root.sh
```

Скрипты делают всё: коннект, root, проверка id/getenforce, поиск пакета, проверка libil2cpp.

### 10. Если не работает root на BlueStacks

1. Включи root в BlueStacks Config:
   - Открой `C:\ProgramData\BlueStacks_nxt\bluestacks.conf`
   - Найди `bst.feature.rooting` = `1`
   - Перезапусти BlueStacks

2. Или через HD-Root:
   ```
   "C:\Program Files\BlueStacks_nxt\HD-Root.exe" -s 5555
   ```

3. Проверь что adbd перезапустился как root - `adb shell id` должен показать `uid=0`

Готово для BlueStacks.
