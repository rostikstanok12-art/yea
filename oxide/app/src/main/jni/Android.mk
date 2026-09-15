LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE    := main
LOCAL_CFLAGS := -std=c17 -fvisibility=hidden -O2
LOCAL_CPPFLAGS := -std=c++17 -fvisibility=hidden -O2 -fexceptions

LOCAL_LDFLAGS += -Wl,--gc-sections,--strip-all -llog
LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/imgui
LOCAL_C_INCLUDES += $(LOCAL_PATH)/imgui/backends
LOCAL_C_INCLUDES += $(LOCAL_PATH)/app

FILE_LIST := $(wildcard $(LOCAL_PATH)/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/app/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/imgui/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/imgui/backends/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/imgui/MyCustom/*.cpp)
LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)

LOCAL_LDLIBS := -llog -landroid -lEGL -lGLESv3 -lz
include $(BUILD_SHARED_LIBRARY)
