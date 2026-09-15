#include <jni.h>
#include "app/配置.h"
extern "C" {
    //初始化opengl窗口
    JNICALL void setSurface(JNIEnv *env, jclass cla, jobject surface) {
        ::window_sv = ANativeWindow_fromSurface(env, surface);
    	std::thread t1(initGui);
        t1.detach();
    }
// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats
    void MotionEventClick(JNIEnv * env, jobject thiz, jint action, jfloat pos_x, jfloat pos_y) {
        static struct ImGuInput_LinuxEvent M_LinuxEvent;
        M_LinuxEvent.Action_type = action;
        M_LinuxEvent.pos.x = pos_x;
        M_LinuxEvent.pos.y = pos_y;
        ::ImGui_ImplLinux_HandleInputEvent(M_LinuxEvent);  
    }

    jstring Start(JNIEnv *env, jclass clazz, jint width, jint height){
        // 程序其他的操作
        _ScreenX = width;
        _ScreenY = height;
        glViewport(0, 0, width, height);
        return env->NewStringUTF("sfg");
    }

    jfloatArray GetImGuiwinsize(JNIEnv * env, jobject thiz) {
        static float winData[4];
        jfloatArray newFloatArray = env->NewFloatArray(4);
        jfloatArray newFloatArray2 = env->NewFloatArray(4);
        static float winData2[4];
        
        if (::Window) {
            winData[0] = ::Window->Pos.x;
            winData[1] = ::Window->Pos.y;
            winData[2] = ::Window->Size.x;
            winData[3] = ::Window->Size.y;
            env->SetFloatArrayRegion(newFloatArray, 0, 4, winData);
            env->ReleaseFloatArrayElements(newFloatArray, winData, JNI_COMMIT);
            return newFloatArray;
        }
        if (::Window2) {
            winData2[0] = ::Window2->Pos.x;
            winData2[1] = ::Window2->Pos.y;
            winData2[2] = ::Window2->Size.x;
            winData2[3] = ::Window2->Size.y;
            env->SetFloatArrayRegion(newFloatArray2, 0, 4, winData2);
            env->ReleaseFloatArrayElements(newFloatArray2, winData2, JNI_COMMIT);
            return newFloatArray2;
        }
        env->ReleaseFloatArrayElements(newFloatArray, winData, JNI_COMMIT);
        env->ReleaseFloatArrayElements(newFloatArray2, winData2, JNI_COMMIT);
        return newFloatArray;
        return newFloatArray2;
    }

    void Set_uuid(JNIEnv *env, jclass clazz, jstring uuid_j) {
        const char *uuid_c = env->GetStringUTFChars(uuid_j, JNI_FALSE);       
		env->ReleaseStringUTFChars(uuid_j, uuid_c);
		}
    void Set_Key(JNIEnv *env, jclass clazz, jstring key_j) {
        char local_stringDatat_input[128];
        const char *str_C = env->GetStringUTFChars(key_j, NULL);
        if (strlen(str_C) >= sizeof(local_stringDatat_input)) {
            memmove((void *)&(local_stringDatat_input), (void *) str_C, 10);                                
            strcat(local_stringDatat_input, "...文件过长");//校验长度
        } else {
            memmove((void *)&(local_stringDatat_input), (void *) str_C, sizeof(local_stringDatat_input));                                
        }
        
    }
    void setPid(JNIEnv *env, jclass clazz, jint pid) {
    }
	
}




// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats




#ifndef NELEM
#define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))
#endif
extern "C" {
    int jniRegisterNativeMethods(JNIEnv *env, const char *className, JNINativeMethod *gMethods, int numMethods) {
        jclass clazz = env->FindClass(className);
        if (clazz == NULL) {
            return JNI_FALSE;
        }
        if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
            return JNI_FALSE;
        }
        return JNI_TRUE;
    }
}
extern "C" {
    static JNINativeMethod native_method_table[] = {
        {"start", "(II)Ljava/lang/String;", (void *) Start},		
        {"setSurface", "(Landroid/view/Surface;)V", (void *) setSurface},		
        {"setPid", "(I)V", (void *) setPid},		
		{"setUUid", "(Ljava/lang/String;)V", (void *) Set_uuid},		
		{"setKey", "(Ljava/lang/String;)V", (void *) Set_Key},		
        {"MotionEventClick", "(IFF)V",  (void*) MotionEventClick},
        {"GetImGuiwinsize", "()[F", (void*) GetImGuiwinsize},		
    };
    
    int register_native_api(JNIEnv *env) {
        return jniRegisterNativeMethods(env, "com/KITE/SuperJNI", native_method_table, NELEM(native_method_table));
    }
}
extern "C" {
    jint JNI_OnLoad(JavaVM *vm, void *reserved) {
        JNIEnv *env;
        if (vm->GetEnv((void **) (&env), JNI_VERSION_1_6) != JNI_OK) {
            return -1;
        }
        assert(env != NULL);
        if (!register_native_api(env)) {//注册接口
            return -1;
        }
        return JNI_VERSION_1_6;
	}
}
// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats