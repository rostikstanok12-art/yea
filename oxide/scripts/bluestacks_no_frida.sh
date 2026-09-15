#!/bin/bash
ADB=${ADB:-adb}
PORT=${1:-127.0.0.1:5555}

echo "[1] connect and root"
$ADB connect $PORT
$ADB -s $PORT root
sleep 2
$ADB connect $PORT
$ADB -s $PORT shell id
$ADB -s $PORT shell setenforce 0
$ADB -s $PORT shell getenforce

echo "[2] push helper SO"
$ADB -s $PORT push oxide/helper/libs/arm64-v8a/liboxide_helper.so /data/local/tmp/
$ADB -s $PORT shell chmod 777 /data/local/tmp/liboxide_helper.so

echo "[3] inject via wrap"
$ADB -s $PORT shell setprop wrap.com.oxide.survivalisland LD_PRELOAD=/data/local/tmp/liboxide_helper.so
$ADB -s $PORT shell am force-stop com.oxide.survivalisland
$ADB -s $PORT shell am start -n com.oxide.survivalisland/com.unity3d.player.UnityPlayerActivity

echo "[4] wait and check dump"
sleep 5
$ADB -s $PORT shell cat /data/local/tmp/oxide_dump.txt
$ADB -s $PORT shell ls -lh /data/local/tmp/oxide*

echo "[5] install ESP overlay"
$ADB -s $PORT install -r oxide/app/build/outputs/apk/debug/app-debug.apk
$ADB -s $PORT shell am start -n com.KITE.OxideEmu/.MainActivity

echo "Done - no Frida"
