LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := injector
LOCAL_SRC_FILES := injector.cpp
LOCAL_CFLAGS := -O2 -fvisibility=hidden
LOCAL_CPPFLAGS := -std=c++17 -O2
LOCAL_LDLIBS := -llog -landroid
include $(BUILD_EXECUTABLE)
