#include <jni.h>
#include "app/config.h"
#include "app/oxide_esp.h"
#include "app/oxide_offsets_bluestacks.h"
#include "app/bluestacks_helper.h"
#include <thread>
#include <android/log.h>

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, "OxideMain", __VA_ARGS__)

int _ScreenX = 0, _ScreenY = 0;
int abs_ScreenX = 0, abs_ScreenY = 0;
bool g_Initialized = false;
ImGuiWindow* Window = nullptr;
ANativeWindow* window_sv = nullptr;
EGLDisplay g_EglDisplay = EGL_NO_DISPLAY;
EGLSurface g_EglSurface = EGL_NO_SURFACE;
EGLContext g_EglContext = EGL_NO_CONTEXT;

OxideSwitch g_Switch;
OxideConfig g_Config;

void initializeEGLContext() {
    const char* g_LogTag = "OxideESP";
    // BlueStacks OpenGL: use 8/8/8/32 for better compatibility with BlueStacks 5 OpenGL renderer
    const EGLint attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_BUFFER_SIZE, 32,
        EGL_DEPTH_SIZE, 16,
        EGL_STENCIL_SIZE, 8,
        EGL_NONE
    };
    ANativeWindow_acquire(window_sv);
    g_EglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (g_EglDisplay == EGL_NO_DISPLAY)
        __android_log_print(ANDROID_LOG_ERROR, g_LogTag, "eglGetDisplay failed - BlueStacks check EGL");
    if (eglInitialize(g_EglDisplay, 0, 0) != EGL_TRUE)
        __android_log_print(ANDROID_LOG_ERROR, g_LogTag, "eglInitialize failed");
    EGLint num_configs = 0;
    eglChooseConfig(g_EglDisplay, attribs, nullptr, 0, &num_configs);
    EGLConfig egl_config;
    eglChooseConfig(g_EglDisplay, attribs, &egl_config, 1, &num_configs);
    EGLint egl_format;
    eglGetConfigAttrib(g_EglDisplay, egl_config, EGL_NATIVE_VISUAL_ID, &egl_format);
    ANativeWindow_setBuffersGeometry(window_sv, 0, 0, egl_format);
    const EGLint ctx_attribs[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };
    g_EglContext = eglCreateContext(g_EglDisplay, egl_config, EGL_NO_CONTEXT, ctx_attribs);
    if (g_EglContext == EGL_NO_CONTEXT)
        __android_log_print(ANDROID_LOG_ERROR, g_LogTag, "eglCreateContext failed - trying ES2");
    if (g_EglContext == EGL_NO_CONTEXT) {
        const EGLint ctx_attribs2[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
        g_EglContext = eglCreateContext(g_EglDisplay, egl_config, EGL_NO_CONTEXT, ctx_attribs2);
    }
    g_EglSurface = eglCreateWindowSurface(g_EglDisplay, egl_config, window_sv, NULL);
    eglMakeCurrent(g_EglDisplay, g_EglSurface, g_EglSurface, g_EglContext);
}

void ImGuiMainWinStart() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplAndroid_NewFrame();
    ImGui::NewFrame();
}

void ImGuiMainWinEnded() {
    ImGui::Render();
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    eglSwapBuffers(g_EglDisplay, g_EglSurface);
}

void initGui() {
    if (g_Initialized) return;
    initializeEGLContext();
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(_ScreenX, _ScreenY);

    ImGui_ImplAndroid_Init();
    ImGui_ImplOpenGL3_Init("#version 300 es");

    ImFontConfig font_cfg;
    font_cfg.SizePixels = 22.0f;
    io.Fonts->AddFontDefault(&font_cfg);

    g_Initialized = true;
    LOGD("ImGui initialized for Oxide BlueStacks OpenGL - Screen %dx%d", _ScreenX, _ScreenY);
    LOGD("BlueStacks check: isBlueStacks=%d", BlueStacksHelper::isBlueStacks());
    BlueStacksHelper::ensurePermissive();

    while (true) {
        if (!window_sv) {
            usleep(100000);
            continue;
        }

        ImGuiMainWinStart();

        if (g_OxideESP.initialized) {
            g_OxideESP.Update();
        }

        extern void DrawOxideMenu();
        DrawOxideMenu();

        if (g_Switch.esp) {
            g_OxideESP.Render();
        }

        ImGuiMainWinEnded();
        usleep(16000);
    }
}

extern "C" {

JNIEXPORT void JNICALL setSurface(JNIEnv *env, jclass cla, jobject surface) {
    window_sv = ANativeWindow_fromSurface(env, surface);
    std::thread t(initGui);
    t.detach();
}

void MotionEventClick(JNIEnv *env, jobject thiz, jint action, jfloat pos_x, jfloat pos_y) {
    static ImGuInput_LinuxEvent ev;
    ev.Action_type = action;
    ev.pos.x = pos_x;
    ev.pos.y = pos_y;
    ImGui_ImplLinux_HandleInputEvent(ev);
}

jstring Start(JNIEnv *env, jclass clazz, jint width, jint height) {
    _ScreenX = width;
    _ScreenY = height;
    abs_ScreenX = width;
    abs_ScreenY = height;
    glViewport(0, 0, width, height);
    return env->NewStringUTF("Oxide BlueStacks ESP Started");
}

jfloatArray GetImGuiwinsize(JNIEnv *env, jobject thiz) {
    static float winData[4];
    jfloatArray arr = env->NewFloatArray(4);
    if (Window) {
        winData[0] = Window->Pos.x;
        winData[1] = Window->Pos.y;
        winData[2] = Window->Size.x;
        winData[3] = Window->Size.y;
        env->SetFloatArrayRegion(arr, 0, 4, winData);
    }
    return arr;
}

void Set_uuid(JNIEnv *env, jclass clazz, jstring uuid_j) {
    const char *c = env->GetStringUTFChars(uuid_j, JNI_FALSE);
    env->ReleaseStringUTFChars(uuid_j, c);
}
void Set_Key(JNIEnv *env, jclass clazz, jstring key_j) {
    const char *c = env->GetStringUTFChars(key_j, NULL);
    env->ReleaseStringUTFChars(key_j, c);
}
void setPid(JNIEnv *env, jclass clazz, jint pid) {
    LOGD("setPid BlueStacks: %d", pid);
    LOGD("Checking root: id check via su");
    system("su -c id");
    system("su -c getenforce");
    g_OxideESP.Init(pid);
    // BlueStacks extra init
    extern void OxideESP_BlueStacks_Init(Memory& mem);
    OxideESP_BlueStacks_Init(g_OxideESP.mem);
}

}

#define NELEM(x) ((int)(sizeof(x)/sizeof((x)[0])))
int jniRegisterNativeMethods(JNIEnv *env, const char *className, JNINativeMethod *gMethods, int numMethods) {
    jclass clazz = env->FindClass(className);
    if (!clazz) return JNI_FALSE;
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) return JNI_FALSE;
    return JNI_TRUE;
}

extern "C" {
    static JNINativeMethod native_method_table[] = {
        {"start", "(II)Ljava/lang/String;", (void*)Start},
        {"setSurface", "(Landroid/view/Surface;)V", (void*)setSurface},
        {"setPid", "(I)V", (void*)setPid},
        {"setUUid", "(Ljava/lang/String;)V", (void*)Set_uuid},
        {"setKey", "(Ljava/lang/String;)V", (void*)Set_Key},
        {"MotionEventClick", "(IFF)V", (void*)MotionEventClick},
        {"GetImGuiwinsize", "()[F", (void*)GetImGuiwinsize},
    };

    int register_native_api(JNIEnv *env) {
        return jniRegisterNativeMethods(env, "com/KITE/SuperJNI", native_method_table, NELEM(native_method_table));
    }

    jint JNI_OnLoad(JavaVM *vm, void *reserved) {
        JNIEnv *env;
        if (vm->GetEnv((void**)(&env), JNI_VERSION_1_6) != JNI_OK) return -1;
        if (!register_native_api(env)) return -1;
        return JNI_VERSION_1_6;
    }
}
