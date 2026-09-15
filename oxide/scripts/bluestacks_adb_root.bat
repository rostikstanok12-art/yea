@echo off
REM BlueStacks + MuMu ADB Root helper for Oxide Panda
REM From user example: adb connect, adb root, check id, getenforce

set ADB=C:\Users\Rostic43\AppData\Local\Android\Sdk\platform-tools\adb.exe
if not exist "%ADB%" set ADB=adb

echo [1] подключение к эмуляторам...
"%ADB%" connect 127.0.0.1:5555
"%ADB%" connect 127.0.0.1:5565
"%ADB%" connect 127.0.0.1:5575
"%ADB%" connect 127.0.0.1:5585
"%ADB%" connect 127.0.0.1:7555
"%ADB%" connect 127.0.0.1:16416
"%ADB%" connect 127.0.0.1:62001

echo.
echo [2] список девайсов...
"%ADB%" devices

echo.
echo [3] поднимаем adbd до root (BlueStacks 5 и MuMu 12 поддерживают из коробки)
for %%p in (127.0.0.1:5555 127.0.0.1:5565 127.0.0.1:5575 127.0.0.1:5585 127.0.0.1:7555 127.0.0.1:16416) do (
    echo --- trying %%p ---
    "%ADB%" -s %%p root
)

timeout /t 2

echo.
echo [4] проверка root
for %%p in (127.0.0.1:5555 127.0.0.1:5565 127.0.0.1:5575 127.0.0.1:5585 127.0.0.1:7555 127.0.0.1:16416) do (
    echo --- %%p ---
    "%ADB%" -s %%p shell id
    "%ADB%" -s %%p shell getenforce
)

echo.
echo [5] ищем Oxide пакет
for %%p in (127.0.0.1:5555 127.0.0.1:5565 127.0.0.1:5575 127.0.0.1:5585 127.0.0.1:7555 127.0.0.1:16416) do (
    echo --- %%p packages ---
    "%ADB%" -s %%p shell pm list packages | findstr oxide
)

echo.
echo [6] проверяем libil2cpp.so base
for %%p in (127.0.0.1:5555 127.0.0.1:7555) do (
    echo --- %%p maps ---
    "%ADB%" -s %%p shell "ps -A | grep oxide"
    "%ADB%" -s %%p shell "cat /proc/$(pidof com.oxide.survivalisland)/maps | grep libil2cpp | head -n 5"
)

echo.
echo Done. Теперь можно запускать Panda overlay.
pause
