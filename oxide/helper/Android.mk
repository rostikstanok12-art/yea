LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := oxide_helper
LOCAL_SRC_FILES := helper.cpp
LOCAL_CFLAGS := -fvisibility=hidden -O2
LOCAL_CPPFLAGS := -std=c++17 -fvisibility=hidden -O2
LOCAL_LDLIBS := -llog -landroid -ldl
include $(BUILD_SHARED_LIBRARY)
