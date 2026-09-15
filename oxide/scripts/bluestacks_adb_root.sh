#!/bin/bash
# BlueStacks + MuMu ADB Root helper for Oxide Panda - Linux/macOS version

ADB=${ADB:-adb}
if ! command -v $ADB &> /dev/null; then
    ADB="$HOME/Android/Sdk/platform-tools/adb"
fi

echo "[1] подключение к эмуляторам..."
$ADB connect 127.0.0.1:5555
$ADB connect 127.0.0.1:5565
$ADB connect 127.0.0.1:5575
$ADB connect 127.0.0.1:5585
$ADB connect 127.0.0.1:7555
$ADB connect 127.0.0.1:16416
$ADB connect 127.0.0.1:62001

echo ""
echo "[2] список девайсов..."
$ADB devices

echo ""
echo "[3] поднимаем adbd до root (BlueStacks 5 и MuMu 12)"
for p in 127.0.0.1:5555 127.0.0.1:5565 127.0.0.1:5575 127.0.0.1:5585 127.0.0.1:7555 127.0.0.1:16416; do
    echo "--- trying $p ---"
    $ADB -s $p root
done

sleep 2

echo ""
echo "[4] проверка root"
for p in 127.0.0.1:5555 127.0.0.1:5565 127.0.0.1:5575 127.0.0.1:5585 127.0.0.1:7555 127.0.0.1:16416; do
    echo "--- $p ---"
    $ADB -s $p shell id || true
    $ADB -s $p shell getenforce || true
done

echo ""
echo "[5] ищем Oxide пакет"
for p in 127.0.0.1:5555 127.0.0.1:7555; do
    echo "--- $p packages ---"
    $ADB -s $p shell pm list packages | grep -i oxide || true
done

echo ""
echo "[6] проверяем libil2cpp.so и делаем permissive"
for p in 127.0.0.1:5555 127.0.0.1:7555; do
    echo "--- $p ---"
    $ADB -s $p shell setenforce 0 || true
    $ADB -s $p shell getenforce
    PID=$($ADB -s $p shell pidof com.oxide.survivalisland || $ADB -s $p shell pidof com.oxide.survival || echo "")
    echo "PID: $PID"
    if [ ! -z "$PID" ]; then
        $ADB -s $p shell "cat /proc/$PID/maps | grep libil2cpp | head -n 5"
    fi
done

echo ""
echo "Done. Теперь запускай Panda overlay."
