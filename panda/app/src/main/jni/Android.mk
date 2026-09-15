LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE    := main
LOCAL_CFLAGS := -std=c17
LOCAL_CFLAGS += -fvisibility=hidden
LOCAL_CPPFLAGS := -std=c++17
LOCAL_CPPFLAGS += -fvisibility=hidden

LOCAL_LDFLAGS += -Wl,--gc-sections,--strip-all, -llog

LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/imgui
LOCAL_C_INCLUDES += $(LOCAL_PATH)/imgui/backends




FILE_LIST += $(wildcard $(LOCAL_PATH)/*.c*)
FILE_LIST += $(wildcard $(LOCAL_PATH)/app/*.c*)
FILE_LIST += $(wildcard $(LOCAL_PATH)/Tools/*.c*)
FILE_LIST += $(wildcard $(LOCAL_PATH)/imgui/*.c*)
FILE_LIST += $(wildcard $(LOCAL_PATH)/imgui/backends/*.c*)
FILE_LIST += $(wildcard $(LOCAL_PATH)/imgui/MyCustom/*.c*)
LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%) 

LOCAL_LDFLAGS := -llog -landroid -lEGL -lGLESv3 
LOCAL_LDFLAGS += -lz
include $(BUILD_SHARED_LIBRARY)
