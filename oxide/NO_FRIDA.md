# Oxide BlueStacks - Работает БЕЗ Frida, полностью на руте

## Как работает без Frida

Старый метод с Frida использовал `frida -U -f` чтобы вызвать `il2cpp_class_from_name` и получить `clientPlayerList`.

Новый метод без Frida - 2 варианта, оба работают на BlueStacks с `adb root`:

### Вариант 1: Helper SO injection (рекомендуется для BlueStacks)

1. **liboxide_helper.so** - маленькая либа, которая при загрузке внутри процесса игры сама вызывает il2cpp API:
   - `il2cpp_domain_get()`
   - `il2cpp_thread_attach(domain)`
   - `il2cpp_domain_assembly_open(domain, "Assembly-CSharp")`
   - `il2cpp_class_from_name(image, "", "PlayerManager")`
   - `il2cpp_class_get_field_from_name(klass, "clientPlayerList")`
   - `il2cpp_field_static_get_value(field, &list)`
   - Пишет адрес листа в `/data/local/tmp/oxide_dump.txt` и `/data/local/tmp/oxide_addrs.bin`

2. **Инжект без Frida** через:
   - `setprop wrap.com.oxide.survivalisland LD_PRELOAD=/data/local/tmp/liboxide_helper.so` - BlueStacks поддерживает wrap, либа загрузится при старте игры
   - Или через ptrace injector `injector` который делает remote dlopen

3. **ESP читает файл**:
   - `oxide_esp.cpp` -> `getPlayerList()` сначала пробует прочитать `/data/local/tmp/oxide_addrs.bin`
   - Если файл есть, парсит List: `size at 0x18`, `items at 0x10`, `array vector at 0x20`
   - Получает массив PlayerManager* и дальше читает позиции через `process_vm_readv`

**Плюс**: не нужен Frida, работает на любом BlueStacks с root, стабильно.

### Вариант 2: Pure external parser (без инжекта вообще)

Если не хочешь инжектить ничего, есть `il2cpp_parser.h`:

- Парсит `/proc/pid/maps` для libil2cpp.so базы
- Сканирует память процесса на наличие Il2CppClass для PlayerManager
- Il2CppClass структура: at 0x10 name ptr, 0x18 namespace ptr, 0xB8 static_fields
- Ищет строку "PlayerManager" в памяти, затем ищет ссылки на нее (классы которые указывают на эту строку)
- Когда находит класс, берет `static_fields = class + 0xB8`, затем `clientPlayerList = static_fields + 0x10`
- Парсит List как в варианте 1

**Плюс**: вообще без инжекта, только `process_vm_readv` и чтение `/proc/pid/maps` и `/proc/pid/mem`
**Минус**: медленнее, нужно сканировать много памяти, может быть нестабильно если il2cpp версия другая.

В `oxide_esp.cpp` реализованы оба метода, пробуются по очереди.

## Файлы для No Frida

- `oxide/helper/helper.cpp` - helper SO, делает il2cpp calls внутри игры
- `oxide/helper/Android.mk` - сборка helper.so
- `oxide/helper/injector.cpp` - ptrace инжектор для BlueStacks (remote dlopen)
- `oxide/app/src/main/jni/app/il2cpp_parser.h` - pure external parser без инжекта
- `oxide/app/src/main/jni/app/oxide_esp.cpp` - теперь сначала читает `/data/local/tmp/oxide_addrs.bin`, потом пробует external parser

## Как собрать helper

```bash
cd oxide/helper
ndk-build
# Получишь libs/arm64-v8a/liboxide_helper.so и libs/x86_64/liboxide_helper.so
adb -s 127.0.0.1:5555 push libs/arm64-v8a/liboxide_helper.so /data/local/tmp/
adb -s 127.0.0.1:5555 shell chmod 777 /data/local/tmp/liboxide_helper.so
```

## Как инжектить на BlueStacks без Frida

### Способ A: wrap (самый простой для BlueStacks)

```bat
adb -s 127.0.0.1:5555 root
adb -s 127.0.0.1:5555 shell setenforce 0
adb -s 127.0.0.1:5555 push liboxide_helper.so /data/local/tmp/
adb -s 127.0.0.1:5555 shell chmod 777 /data/local/tmp/liboxide_helper.so
adb -s 127.0.0.1:5555 shell setprop wrap.com.oxide.survivalisland LD_PRELOAD=/data/local/tmp/liboxide_helper.so
adb -s 127.0.0.1:5555 shell am force-stop com.oxide.survivalisland
adb -s 127.0.0.1:5555 shell am start -n com.oxide.survivalisland/com.unity3d.player.UnityPlayerActivity
adb -s 127.0.0.1:5555 shell sleep 3 && cat /data/local/tmp/oxide_dump.txt
```

Должно показать:
```
clientPlayerList=0x7a3b...
activePlayerList=0x7a3b...
```

### Способ B: ptrace injector (live, без рестарта)

```bat
adb -s 127.0.0.1:5555 push injector /data/local/tmp/
adb -s 127.0.0.1:5555 shell chmod 777 /data/local/tmp/injector
adb -s 127.0.0.1:5555 shell su -c "/data/local/tmp/injector com.oxide.survivalisland /data/local/tmp/liboxide_helper.so"
adb -s 127.0.0.1:5555 shell cat /data/local/tmp/oxide_dump.txt
```

### Способ C: pure external (вообще без инжекта)

Если не хочешь ничего инжектить, просто запусти ESP overlay:
- Он сам попробует найти PlayerManager через сканирование памяти
- Работает медленнее, но без файлов в /data/local/tmp
- Для BlueStacks с root и Permissive должно работать

## Проверка что работает без Frida

```bat
adb -s 127.0.0.1:5555 shell su -c "ls -lh /data/local/tmp/oxide*"
adb -s 127.0.0.1:5555 shell su -c "cat /data/local/tmp/oxide_dump.txt"
adb -s 127.0.0.1:5555 shell su -c "hexdump -C /data/local/tmp/oxide_addrs.bin | head"

adb -s 127.0.0.1:5555 logcat -s OxideHelper:D OxideESP:D
```

Если видишь в logcat:
```
OxideHelper: clientPlayerList=0x...
OxideESP: Got 10 players from helper file list
```
Значит работает без Frida.

## Итог

- Frida больше не нужна
- Для BlueStacks достаточно `adb root` + `setenforce 0` + helper SO
- Helper SO пишет адрес листа, ESP читает его через файл и дальше через process_vm_readv
- Все оффсеты уже подставлены (0x1C8 pos, 0x10 clientList, 0x18 size и тд)

Готово для BlueStacks без Frida.
