# Oxide BlueStacks - Чистый No Inject, только рут

## Запрос: "можно без inject?"

Да, теперь полностью без инжекта, без Frida, без helper SO, без LD_PRELOAD, без ptrace.

Только:
- `adb root` (uid=0)
- `setenforce 0` (Permissive)
- `process_vm_readv` syscall 270/271
- чтение `/proc/pid/maps` и `/proc/pid/mem`

## Как работает Pure External No Inject

### Идея

В BlueStacks с рутом мы можем читать память любого процесса через `process_vm_readv`, даже без инжекта.

Осталось найти где в памяти лежит `clientPlayerList` (List<PlayerManager>).

### Реализация в `pure_external.h`

**Метод 1: Поиск s_TypeInfo массива (самый точный)**

1. Из дампа знаем TypeDefIndex для PlayerManager = 9223
2. В libil2cpp.so есть глобальный массив `s_TypeInfo` - массив указателей на Il2CppClass, индекс = TypeDefIndex
3. Находим libil2cpp.so .data секцию (rw-p) через `/proc/pid/maps`
4. Сканируем .data на предмет массива, где элемент 9223 указывает на класс с именем "PlayerManager"
   - Il2CppClass: name at 0x10 -> pointer to string "PlayerManager"
   - Проверяем: читаем klass + 0x10 = namePtr, читаем строку по namePtr, сравниваем с "PlayerManager"
5. Когда нашли класс, берем static_fields:
   - static_fields at klass + 0xB8 (или 0xC0, 0xC8 - пробуем все)
6. clientPlayerList = static_fields + 0x10, activePlayerList = static_fields + 0x8
7. Парсим List:
   - size at list + 0x18
   - items at list + 0x10 (Il2CppArray)
   - maxLength at array + 0x18
   - vector at array + 0x20, элементы = vector + i*8

**Метод 2: Brute force скан хипа (fallback)**

Если метод 1 не сработал (другая версия il2cpp):

1. Сканируем все rw регионы ([heap], [anon]) на наличие List объектов
2. List объект: size at 0x18 должен быть 1..100, items at 0x10 должен быть валидным указателем
3. items = Il2CppArray, maxLength at 0x18 должен быть >= size и <= 200
4. Для каждого элемента массива проверяем, похож ли он на PlayerManager:
   - lastTickPosition at 0x1C8 = Vector3 с координатами в пределах -10000..10000, не NaN
   - vitals at 0xC8 = валидный указатель
5. Если нашли List с 1..100 валидных PlayerManager - это наш player list

Оба метода работают без инжекта, только чтение памяти.

### Код

В `oxide_esp.cpp`:
```cpp
std::vector<uintptr_t> OxideESP::getPlayerList(bool activeOnly) {
    // PRIORITY 1: Pure external NO INJECT
    {
        PureExternal pure(&mem);
        auto players = pure.getPlayersPureExternal();
        if (!players.empty()) return players;
    }
    // fallback to helper file etc...
}
```

`PureExternal::getPlayersPureExternal()` делает все выше.

## Что нужно для работы на BlueStacks без инжекта

1. **Root**:
```bat
adb connect 127.0.0.1:5555
adb -s 127.0.0.1:5555 root
adb -s 127.0.0.1:5555 shell id
# uid=0(root)
```

2. **Permissive**:
```bat
adb -s 127.0.0.1:5555 shell setenforce 0
adb -s 127.0.0.1:5555 shell getenforce
# Permissive
```

3. **Запустить Oxide**:
```bat
adb -s 127.0.0.1:5555 shell am start -n com.oxide.survivalisland/com.unity3d.player.UnityPlayerActivity
```

4. **Установить и запустить ESP**:
```bat
adb -s 127.0.0.1:5555 install -r app-debug.apk
adb -s 127.0.0.1:5555 shell am start -n com.KITE.OxideEmu/.MainActivity
```

5. **В приложении нажать Start ESP Overlay** - все, без инжекта.

В логах должно быть:
```
OxideESP: PureExternal no-inject - il2cpp base: 7a3b...
PureExternal: Found PlayerManager class at 7a3b..., s_TypeInfo base at 7a3b...
PureExternal: got 10 players from clientList
```

## Проверка без инжекта

```bat
adb -s 127.0.0.1:5555 shell su -c "cat /proc/$(pidof com.oxide.survivalisland)/maps | grep libil2cpp"
adb -s 127.0.0.1:5555 logcat -s PureExternal:D OxideESP:D
```

Если видишь "Found PlayerManager class" - значит pure external сработал без инжекта.

## Плюсы No Inject

- Не нужен helper SO
- Не нужен wrap / ptrace / Frida
- Не нужно рестартить игру
- Работает сразу после `adb root`
- Не детектится как инжект (только чтение памяти через syscall, как GameGuardian)
- Для BlueStacks идеально, т.к. root из коробки

## Минусы

- Медленнее чем helper (сканирование памяти занимает 1-2 сек)
- Может не сработать если il2cpp версия сильно другая (тогда нужно обновить offset static_fields 0xB8/0xC0)
- W2S пока заглушка - нужно дописать чтение viewMatrix из Camera.main (тоже можно без инжекта через скан)

## Оффсеты для No Inject (уже подставлены)

```
PlayerManager:
  lastTickPosition 0x1C8
  vitals 0xC8
  static clientPlayerList 0x10
  static activePlayerList 0x8

List:
  items 0x10
  size 0x18

Array:
  maxLength 0x18
  vector 0x20

Il2CppClass:
  name 0x10
  static_fields 0xB8 (пробуем 0xB8,0xC0,0xC8)

TypeDefIndex PlayerManager = 9223
```

Все уже в коде.

## Итог

Теперь можно без инжекта вообще. Только рут через adb как ты показал.

Если хочешь еще быстрее - могу добавить кэширование s_TypeInfo базы в файл `/data/local/tmp/oxide_class_cache.bin` чтобы второй раз не сканировать.
