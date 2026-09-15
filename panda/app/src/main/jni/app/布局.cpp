#include "配置.h"
#include "imgui/字体.h"
#include "读写.h"
#include "Vector3.hpp"
#include <vector>
#include <cmath>
#include "wy/wy.h"
//MyDraw myDraw;
读写 读写;
配置 配置;
开关 按钮;

bool TL = true;
const char *imei;
char password[30] = {0};
static std::string inputText = password; // 用于显示输入文本

// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats
constexpr float PI = 3.14159265358979323846f;
constexpr float TWO_PI = 2.0f * PI;

static long int handle;
float WriteAddress_FLOAT(long int addr, float value) {
	pwrite64(handle, &value, 4, addr);
	return 0;
}
int Pid;
uintptr_t Uworld,Matrix1,Matrix2,libbase,lib2cppbss,模块xa;
int Screen_px = 0, Screen_py = 0;
int _ScreenX = 0, _ScreenY = 0;
int abs_ScreenX = 0, abs_ScreenY = 0;
bool g_Initialized = false;
ImGuiWindow *Window = nullptr;
ImGuiWindow *Window2 = nullptr;
ANativeWindow *window_sv = nullptr;
EGLDisplay g_EglDisplay = EGL_NO_DISPLAY;
EGLSurface g_EglSurface = EGL_NO_SURFACE;
EGLContext g_EglContext = EGL_NO_CONTEXT;
int MenuTab = 1;
ImTextureID FloatBall;
void ImGuiLayout1();
void ImGuiLayout2();
void ImGuiLayout3();
void ImGuiLayout4();
void initializeEGLContext() {
    const char *g_LogTag = "ImGuiExample";
    const EGLint attribs1[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_BLUE_SIZE, 5,   //-->delete
        EGL_GREEN_SIZE, 6,  //-->delete
        EGL_RED_SIZE, 5,    //-->delete
        EGL_BUFFER_SIZE, 32,  //-->new field
        EGL_DEPTH_SIZE, 16,
        EGL_STENCIL_SIZE, 8,
        EGL_NONE
    };

	ANativeWindow_acquire(::window_sv);
	g_EglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if (g_EglDisplay == EGL_NO_DISPLAY)
		__android_log_print(ANDROID_LOG_ERROR, g_LogTag, "%s","eglGetDisplay(EGL_DEFAULT_DISPLAY) returned EGL_NO_DISPLAY");
	if (eglInitialize(g_EglDisplay, 0, 0) != EGL_TRUE)
		__android_log_print(ANDROID_LOG_ERROR, g_LogTag, "%s","eglInitialize() returned with an error");
	EGLint num_configs = 0;
	if (eglChooseConfig(g_EglDisplay, attribs1, nullptr, 0, &num_configs) != EGL_TRUE)
		__android_log_print(ANDROID_LOG_ERROR, g_LogTag, "%s","eglChooseConfig() returned with an error");
	if (num_configs == 0)
		__android_log_print(ANDROID_LOG_ERROR, g_LogTag, "%s","eglChooseConfig() returned 0 matching config");
	EGLConfig egl_config;
	eglChooseConfig(g_EglDisplay, attribs1, &egl_config, 1, &num_configs);
	EGLint egl_format;
	eglGetConfigAttrib(g_EglDisplay, egl_config, EGL_NATIVE_VISUAL_ID, &egl_format);
	ANativeWindow_setBuffersGeometry(window_sv, 0, 0, egl_format);
	const EGLint egl_context_attributes[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };
	g_EglContext = eglCreateContext(g_EglDisplay, egl_config, EGL_NO_CONTEXT,egl_context_attributes);
	if (g_EglContext == EGL_NO_CONTEXT)
		__android_log_print(ANDROID_LOG_ERROR, g_LogTag, "%s","eglCreateContext() returned EGL_NO_CONTEXT");
	g_EglSurface = eglCreateWindowSurface(g_EglDisplay, egl_config, window_sv, NULL);
	eglMakeCurrent(g_EglDisplay, g_EglSurface, g_EglSurface, g_EglContext);

	
	}
// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats

bool Android_LoadSystemFont() {
    char path[64]{0};
    char *filename = nullptr;
    const char *fontPath[] = { "/system/fonts", "/system/font", "/data/fonts" };
    for (auto tmp:fontPath) {
        if (access(tmp, R_OK) == 0) {
            strcpy(path, tmp);
            filename = path + strlen(tmp);
            break;
        }
    }
    if (!filename) {
        return false;
    }
    *filename++ = '/';
    strcpy(filename, "NotoSansCJK-Regular.ttc");
    if (access(path, R_OK) != 0) {
        strcpy(filename, "NotoSerifCJK-Regular.ttc");
        if (access(path, R_OK) != 0) {
            return false;
        }
    }
}
// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats
/************用户自定义**************/
void ImGuiMainWinStart() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplAndroid_NewFrame();
	ImGui::NewFrame();
}
void ImGuiMainWinEnded() {
 	ImGui::Render();
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    if (eglSwapBuffers(g_EglDisplay, g_EglSurface)) {
    }
}
ImColor 随机颜色[1000] = {};
ImColor 随机颜色样 = {};
void initGui() {
    if (g_Initialized) {
        return;
    }
	initializeEGLContext();
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
	for (auto & i : 随机颜色) {
        i = ImColor(ImVec4((rand() % 205 + 50) / 255.f, rand() % 255 / 255.f, rand() % 225 / 225.f, 225 / 225.f));
    }
// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats
	ImGui::StyleColorsLight();//白色
    ImGui_ImplAndroid_Init(window_sv);
    ImGui_ImplOpenGL3_Init("#version 300 es");
	io.Fonts->AddFontFromMemoryTTF((void *) font_v, font_v_size, 24.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
	IM_ASSERT(io.Fonts != NULL);  
     IM_ASSERT(font != nullptr);
    
	ImGui::GetStyle().ScaleAllSizes(3.0f);
    g_Initialized = true;
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    
    //菜单函数
    ImGuiStyle &style = ImGui::GetStyle();
    style.ItemSpacing=ImVec2(4.0f, 2.0f);//设置控件之间的间距为(
    
    abs_ScreenX = (_ScreenX > _ScreenY ? _ScreenX : _ScreenY);
    abs_ScreenY = (_ScreenX < _ScreenY ? _ScreenX : _ScreenY);    
    Screen_px = abs_ScreenX / 2;
    Screen_py = abs_ScreenY / 2;
  
    while(true) {
	    ::ImGuiMainWinStart();	    
        ::ImGuiMainMenuWind();//布局
        ::ImGuiMainWinEnded();   	     
    }
}
// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats
struct Angles {
   float yaw, pitch;
};
Angles CalculateViewAngle(const Vector3& myPlayer, const Vector3& target) {
  
    Angles viewangl;
    
    float centertotop = -60.0f; 
    float centertobottom = 60.0f; 

    float directionX = target.X - myPlayer.X;
    float directionY = target.Y - myPlayer.Y;
    float directionZ = target.Z - myPlayer.Z;
    
    float angleRadiansy = atan2f(directionY, sqrtf(directionX * directionX + directionZ * directionZ));
    
    float angleDegreesy = angleRadiansy * (180.0f / M_PI);
    
    float yAngle = -angleDegreesy; 
    yAngle = fmaxf(centertotop, fminf(centertobottom, yAngle));
    viewangl.pitch = yAngle;
    
    float angleRadiansx = atan2f(directionX, directionZ);
    
    float angleDegreesx = angleRadiansx * (180.0f / M_PI);
    
    float xAngle = fmodf((angleDegreesx + 360.0f), 360.0f); //its 360 like 0 - 360 center to right umm having whole rotation            
    viewangl.yaw = xAngle;
    // 计算时间差
    return viewangl;
}
// 计算相对角度差的核心函数
static void calculateAngleDelta(
    const Vector3& relativePos,
    const Angles& crosshairAngles,
    float& outDeltaYaw,
    float& outDeltaPitch) 
{
    // 计算水平角度（yaw）
    const float enemyYaw = std::atan2(relativePos.Z, relativePos.X);
    outDeltaYaw = enemyYaw - crosshairAngles.yaw;
    
    // 处理水平角度环绕（确保差值在[-π, π]范围内）
    if (outDeltaYaw > PI) {
        outDeltaYaw -= TWO_PI;
    } else if (outDeltaYaw < -PI) {
        outDeltaYaw += TWO_PI;
    }
// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats
// СЛИЛ @uintptrcode | @GlowCheats
    // 计算垂直角度（pitch）
    const float distance2D = std::sqrt(relativePos.X*relativePos.X + relativePos.Z*relativePos.Z);
    const float enemyPitch = std::atan2(relativePos.Y, distance2D);
    outDeltaPitch = enemyPitch - crosshairAngles.pitch;
}

#define IM_PI 3.14159265358979323846f
#define RAD2DEG(x) ((float)(x) * (float)(180.f / IM_PI))
#define DEG2RAD(x) ((float)(x) * (float)(IM_PI / 180.f))    
// 计算两点之间的距离
float calculateDistance(const Vector3& PlayerPos, const Vector3& MyPos) {
    float dx = PlayerPos.X - MyPos.X;
    float dy = PlayerPos.Z - MyPos.Z;
    return sqrt(dx * dx + dy * dy);
}

float calculateAngle(const Vector3& PlayerPos, const Vector3& MyPos, const float& myAngle) {
    // 计算相对位置
    float dx = PlayerPos.X - MyPos.X;
    float dy = PlayerPos.Z - MyPos.Z;
    
    // 计算相对角度（弧度）并转换为度
  //  float angle = std::atan2(dy, dx) * 180.0f / M_PI;
    
    // 将玩家的当前角度加到相对角度上
   // angle += myAngle;
    
    // 将角度调整到 [0, 360) 范围内
   /* if (angle >= 360.0f) {
        angle -= 360.0f;
    } else if (angle < 0) {
        angle += 360.0f;
    }*/
    
     // Calculate the angle in radians
    float angleRadiansx = atan2f(dx, dy);
    // Convert the angle from radians to degrees
    float angleDegreesx = angleRadiansx * (180.0f / M_PI);
    // Adjust the angle to fit our desired range (0 to 360)
    float xAngle = fmodf((angleDegreesx + 360.0f), 360.0f); //its 360 like 0 - 360 center to right umm having whole rotation            
    float angle = xAngle;
    
    return angle;
}











uintptr_t 数组;
uintptr_t 开镜判断;
uintptr_t 对象指针;
bool 准心最近;
bool 距离最近;

Vector3 距离最近坐标;
Vector3 准心最近坐标;
   
void QingZ (ImDrawList *绘制){
    float py = abs_ScreenY/2;  
    float px = abs_ScreenX/2;
   
	  
    float matrix[16] = { 0 };
    if (按钮.绘制){
	读写.Initializeread(Pid);//游戏pid
	}	
	Pid = 读写.getPID("com.OneManEmpire.SurvivalOdyssey");//包名    
	libbase = 读写.getbss("libunity.so");//模块
	lib2cppbss = 读写.getbss("libil2cpp.so");//模块
	
	uintptr_t 矩阵 = 读写.getPtr64(读写.getPtr64(读写.getPtr64(libbase + 0x1CAE0)+0xA0)+0x3E0)+0x3FC;//0x1CAE0+0xA0+0x3E0+0x3FC
	数组 = 读写.getPtr64(读写.getPtr64(libbase +0xED458)+0x3F8)+0x0;//0xED458+0x3F8+0x0
	//libunity.so:bss+0xE4FD8+0x3F8+0x0
   uintptr_t 开镜地址 = 读写.getPtr64(读写.getPtr64(读写.getPtr64(读写.getPtr64(读写.getPtr64(libbase +0x6a4f689010)+0x3C0)+0x190)+0x390)+0x208)+0x10;
   //0x7F3B0+0x3C0+0x190+0x390+0x208+0x10
    uintptr_t 自身头 = 读写.getPtr64(读写.getPtr64(读写.getPtr64(读写.getPtr64(libbase + 0x1C160)+0xA0)+0x80)+0x188)+0x90;//0x1C160+0xA0+0x80+0x188+0x90
    uintptr_t 准心地址 = 读写.getPtr64(读写.getPtr64(读写.getPtr64(libbase +0x7F798)+0x4A8)+0x78)+0x60;//0x7F798+0x4A8+0x78+0x60
	//xa 48B01B8
    //bss 第1级偏移 0x2321B8
    //libunity.so:bss+0xCB08+0x240+0x2B0+0x150+0x90
   float closestDistance = std::numeric_limits<float>::max();
   float minAngleSquared = std::numeric_limits<float>::max();                 
   float minDistanceSquared = std::numeric_limits<float>::max(); // 初始化为最大浮点数
   float Xs;//最近坐标转屏幕坐标
   float Ys;//最近坐标转屏幕坐标
        
  
   Vector3 自身;
   自身.X = 读写.getFloat(自身头);
   自身.Y = 读写.getFloat(自身头+0x4);
   自身.Z = 读写.getFloat(自身头+0x8); 
   
   
   if(按钮.雷达){
   struct Vector3 Pos;
    Pos.X = abs_ScreenX / 4.395;
    Pos.Y = abs_ScreenY / 300;
    struct Vector3 Size;
    Size.X = 配置.雷达X; // عرض صندوق الرادار
    Size.Y = 配置.雷达Y; // ارتفاع صندوق الرادار
    float RadarCenterX = Pos.X + (Size.X / 2);
    float RadarCenterY = Pos.Y + (Size.Y / 2);
    ImColor CircleColor = ImColor(0, 255, 0, 130); // لون خلفية الرادار
    ImColor EnemyColor = ImColor(255, 0, 0, 255); // لون النقاط الحمراء للأعداء
    ImColor FriendColor = ImColor(0, 255, 0, 255); // لون النقاط الخضراء للأصدقاء
    ImColor TeamColor = ImColor(185,185,185,255); // لون النقاط الزرقاء للفريق
    ImColor RadarLineColor = ImColor(0, 255, 0, 100); // لون خط الرادار المتحرك

    //背景图
    ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2(RadarCenterX + 0.5f, RadarCenterY + 0.5f), 150.f, ImColor(255, 255, 255, 50), 1000);
    ImGui::GetBackgroundDrawList()->AddLine(ImVec2(RadarCenterX, RadarCenterY),
                          ImVec2(RadarCenterX + 170, RadarCenterY),
                          ImColor(255, 255, 255, 255), 1.f);  
    ImGui::GetBackgroundDrawList()->AddLine(ImVec2(RadarCenterX, RadarCenterY),
                          ImVec2(RadarCenterX - 170, RadarCenterY),
                          ImColor(255, 255, 255, 255), 1.f);    
   ImGui::GetBackgroundDrawList()->AddLine(ImVec2(RadarCenterX, RadarCenterY),
                          ImVec2(RadarCenterX, RadarCenterY + 170),
                          ImColor(255, 255, 255, 255), 1.f);    
   ImGui::GetBackgroundDrawList()->AddLine(ImVec2(RadarCenterX, RadarCenterY),
                          ImVec2(RadarCenterX, RadarCenterY - 170),
                          ImColor(255, 255, 255, 255), 1.f);         


    //循环绘制雷达图
     for (float i = 50.f; i <= 150.f; i += 50.f) {
        ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(RadarCenterX, RadarCenterY), i, ImColor(255, 255, 255, 255), 1000);
    }                                                                                                                                                                                                                                                                                                                                                                                                   
    //自身圆点
    ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2(RadarCenterX + 0.5f, RadarCenterY + 0.5f), 3.f, ImColor(255, 255, 255, 255), 1000);
           
    ImColor NumberColor = ImColor(255, 255, 255, 255);
    // 方向刻度（0°在上方）
for (int i = 0; i < 360; i += 90) {
    float adjustedAngle = i - 90; // 0° -> 上方
    float radian = adjustedAngle * (M_PI / 180.0f);
    float textX = RadarCenterX + 180.f * cos(radian);
    float textY = RadarCenterY + 180.f * sin(radian);
    char numberText[10];
    sprintf(numberText, "%d°", i); // 或使用 (i + 270) % 360 避免负值
    ImGui::GetBackgroundDrawList()->AddText(ImVec2(textX, textY), NumberColor, numberText);
}
        
    float angle3 = fmod(读写.getFloat(准心地址+0x4), 360.0f); // تعديل السرعة حسب الحاجة
    float radians = angle3 * (M_PI / 180.0f);
    
    // زاوية القطاع الدائري
    float sectorAngle = 120.0f * (M_PI / 180.0f); // 30 درجة كقطاع دائري
    float startAngle = radians - (sectorAngle / 2);
    float endAngle = radians + (sectorAngle / 2);
    
    // رسم القطاع الدائري
    ImGui::GetBackgroundDrawList()->PathClear();
    ImGui::GetBackgroundDrawList()->PathLineTo(ImVec2(RadarCenterX, RadarCenterY));
    for (int i = 0; i <= 100; i++) {
        float t = (float)i / 100;
        float ang = startAngle + t * (endAngle - startAngle);
        ImGui::GetBackgroundDrawList()->PathLineTo(ImVec2(RadarCenterX + 150.f * cos(ang), RadarCenterY + 150.f * sin(ang)));
    }
    ImGui::GetBackgroundDrawList()->PathFillConvex(RadarLineColor);
   
    float time = ImGui::GetTime();
    float speed = 100.0f; // سرعة دوران الخط
    float angle = fmod(time * speed, 360.0f); // تعديل السرعة حسب الحاجة
    float radians222 = angle * (M_PI / 180.0f);   
    float lineLength = 150.0f; // The length of the line 
    float endX = RadarCenterX + lineLength * cos(radians222);
    float endY = RadarCenterY + lineLength * sin(radians222);                           
    
   ImGui::GetBackgroundDrawList()->AddLine(ImVec2(RadarCenterX, RadarCenterY),
                          ImVec2(endX, endY),
                          ImColor(255,0,0), 3.0f);
         
            
          
   }
   

   
	memset(matrix, 0, 16);
	读写.readv(矩阵, matrix, 16 * 4);
	for (int i = 0; i < 12000; i++)	// 绘制数量
	{
     对象指针 = 读写.getPtr64(数组+8 * i);//数组偏移
		
	//    int TeamID = 读写.getDword(对象指针 + 0x4c); 
  //      int PlayerName = 读写.getPtr64(对象指针 + 0x4c8); 
		float Health = 100;//血量
			
		
		if (按钮.人)
		{
		float wzzb = 读写.getDword(对象指针 + 0x90);
	
		   if (wzzb != 116)
			{
				continue;
			}
		}
		//绘制用的
		Vector3 敌人;
		读写.readv(对象指针+0xA0, &敌人, sizeof(敌人));// 对象坐标X轴
		敌人.X=读写.getFloat(对象指针+0xA0);
        敌人.Y=读写.getFloat(对象指针+0xA4);
        敌人.Z=读写.getFloat(对象指针+0xA8);
				
		
		
		
	//人物自瞄用的
		Vector3 瞄准敌人;
		读写.readv(对象指针+0xA0, &瞄准敌人, sizeof(瞄准敌人));// 对象坐标X轴
		瞄准敌人.X=读写.getFloat(对象指针+0xA0);
        瞄准敌人.Y=读写.getFloat(对象指针+0xA4);
        瞄准敌人.Z=读写.getFloat(对象指针+0xA8);
// 调整 Y 坐标，使自瞄瞄准身体
float 身体偏移 = 配置.自瞄位置;
      瞄准敌人.Y -= 身体偏移;      // 让自瞄瞄准身体位置


    
        
        
        
        
        
      /*  
   	    float playerDistance = Vector3::Distance(自身,敌人);  
        if (playerDistance<1.6 || playerDistance>240)
		{
			continue;
		}       */

        
		
		float camera = matrix[3] * 敌人.X + matrix[7] * 敌人.Y + matrix[11] * 敌人.Z + matrix[15];
		float r_x = px + (matrix[0] * 敌人.X + matrix[4] *(敌人.Y) + matrix[8] * 敌人.Z + matrix[12]) / camera * px;	// 视角高
		float r_y = py - (matrix[1] * 敌人.X + matrix[5] *(敌人.Y + 1) + matrix[9] * 敌人.Z + matrix[13]) / camera * py;
		float r_w =py - (matrix[1] * 敌人.X + matrix[5] *(敌人.Y + 3) + matrix[9] * 敌人.Z + matrix[13]) / camera * py;
				  
	    float X = r_x- (r_y - r_w) / 35;
		float Y = r_y;
		float     W= (r_y - r_w) / 2;
		float 距离 = camera / 1;
		float XLX = r_x - (r_y - r_w) / 85;
		float top = Y - W;
		float bottom = Y + W;
		float MIDDLE = X + W / 2;
		float left = MIDDLE - (W / 2);
		float right = MIDDLE + (W / 2);
		float pmjl = sqrt(pow(r_x - px, 2) + pow(r_y - py, 2));
		float aim_distance = sqrt(pow(r_x , 2) + pow(r_y , 2));
		float x1 = X + W / 2; 
		
		float playerDistance = Vector3::Distance(自身,瞄准敌人);  
        if (playerDistance<1.6 || playerDistance>300)
		{
			continue;
		}
		
      float distanceSquared = (X - px) * (X - px) + (Y - py) * (Y - py);
       if (playerDistance < closestDistance) {
           距离最近坐标 = 瞄准敌人;
        closestDistance = playerDistance;         
           Ys = top;
           Xs = x1;
         
           
        }
       if (distanceSquared < minDistanceSquared) {
           准心最近坐标 = 瞄准敌人;
           minDistanceSquared = distanceSquared;
           closestDistance = playerDistance;         
           Ys = top;
           Xs = x1;
           
           
		}
        
		

       
		
		
		if (W > 0)
		{
			if (按钮.方框){
				ImGui::GetForegroundDrawList()->AddRect({X - (W / 2), top}, {X + (W / 2), bottom}, ImColor(255, 0, 0), {2}, 0, {2});
		
			}
			if (按钮.射线){
				ImGui::GetForegroundDrawList()->AddLine({px, 2}, {r_x, top - 0}, ImColor(255, 255, 255), {1});
				
				//ImGui::GetForegroundDrawList()->AddLine({px/2, py/73.5}, {MIDDLE, top - 0}, ImColor(255, 255, 255), {1});
				
			}
			if (按钮.距离) {
				char Tkilo[256];
				std::string s;
				s += std::to_string((int)距离);  // 强制转为整数，去掉小数
                s += "m";
				auto textSize = ImGui::CalcTextSize(s.c_str(), 0, 28.f);
				ImGui::GetForegroundDrawList()->AddText(NULL, 25.f,{X, bottom + 15}, ImColor(255,255,255), s.c_str());
			}    			
			
					
			    
			    
			    
			    
			
			
			    
			    
			    
			    
			    
			    
			    
            
	   
	    if (按钮.血量)
          {
ImColor 血量颜色;
if (Health < 35)
血量颜色 = ImColor(204, 51, 51, 200);
else if (Health < 75)
血量颜色 = ImColor(255, 153, 153, 200);
else
血量颜色 = ImColor(0, 255, 0);
ImGui::GetForegroundDrawList()->AddRectFilled({X - px / 80, top - py / 22}, {X - px / 19 + (2.1f * px / 21) * Health /100, Y-W - py / 26},血量颜色, {0.5});
        }
			//                                 左边长                高
			
		    if (按钮.自瞄圈)
        {
        ImColor 圈圈颜色 = ImColor(255, 0, 0);        
        ImGui::GetForegroundDrawList()->AddCircle({px, py},配置.自瞄范围, 圈圈颜色,0,2.5f);
        }                
   
	
		
		}
		if (按钮.雷达) {   
    struct Vector3 Pos;
    Pos.X = abs_ScreenX / 4.395;
    Pos.Y = abs_ScreenY / 300;
    struct Vector3 Size;
    Size.X = 配置.雷达X; // عرض صندوق الرادار
    Size.Y = 配置.雷达Y; // ارتفاع صندوق الرادار
    float RadarCenterX = Pos.X + (Size.X / 2);
    float RadarCenterY = Pos.Y + (Size.Y / 2);
    ImColor CircleColor = ImColor(0, 255, 0, 130); // لون خلفية الرادار
    ImColor EnemyColor = ImColor(255, 0, 0, 255); // لون النقاط الحمراء للأعداء
    ImColor FriendColor = ImColor(0, 255, 0, 255); // لون النقاط الخضراء للأصدقاء
    ImColor TeamColor = ImColor(185,185,185,255); // لون النقاط الزرقاء للفريق
    ImColor RadarLineColor = ImColor(0, 255, 0, 100); // لون خط الرادار المتحرك            
            // 计算距离和角度
            float distance2 = calculateDistance(敌人, 自身);
            float angle222 = calculateAngle(敌人, 自身, 读写.getFloat(准心地址+0x4));


            // 调整雷达坐标的比例
            float deltaX = distance2 * cos(angle222 * M_PI / 180.0f);
            float deltaY = distance2 * sin(angle222 * M_PI / 180.0f);
            float RadarPosX = RadarCenterX + (deltaX * 0.6); // 调整比例因子
            float RadarPosY = RadarCenterY + (deltaY * 0.6); // 调整比例因子
                 
            ImColor PointColor = EnemyColor;                        
            ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2(RadarPosX, RadarPosY), 8.f, PointColor, 1000);

            // رسم المسافة
            char distanceText[25];
            sprintf(distanceText, "%0.0f m", playerDistance);          
            ImGui::GetBackgroundDrawList()->AddText(ImVec2(RadarPosX, RadarPosY), ImGui::ColorConvertFloat4ToU32(PointColor), distanceText);
            

            // رسم اسم اللاعب
                      
            char nick[0xFF] = {0};
            
            ImColor NameColor = ImColor(255, 0, 0, 255);
            //ImGui::GetBackgroundDrawList()->AddText(ImVec2(RadarPosX - 5.f, RadarPosY - 18.f), ImGui::ColorConvertFloat4ToU32(NameColor), nick);            
        }
	}
	 
	
	  /* if (按钮.自瞄) {
	   
	   if (closestDistance >= 1.6f && closestDistance <= 配置.自瞄距离) {
	   Angles angles = CalculateViewAngle(自身, 按钮.最近 ? 距离最近坐标 : 准心最近坐标);
            angles.pitch += 配置.自瞄位置; // 应用垂直偏移
            
           
	   
	   
	   if (closestDistance >= 1.6) {
	    if (准心最近) {	    	
	    Angles angles222 = CalculateViewAngle(自身,准心最近坐标);     
        读写.writefloat(准心地址, angles.pitch);
	    读写.writefloat(准心地址+0x4, angles222.yaw); 	    	    
	    }	 
	    if (距离最近) {	    	    
        Angles angles222 = CalculateViewAngle(自身, 距离最近坐标);
	    读写.writefloat(准心地址, angles.pitch);
	    读写.writefloat(准心地址+0x4, angles222.yaw);                  
         }
      }
   }
   */
   
   
   
   if (按钮.自瞄) {   
  
   if (closestDistance >= 1.6f && closestDistance <= 配置.自瞄距离) {
        Angles angles = CalculateViewAngle(自身, 按钮.最近 ? 距离最近坐标 : 准心最近坐标);
    //    angles.pitch += 配置.自瞄位置;        //准星Y
        // 更安全的开镜状态读取方式      
        if (按钮.开镜) {	    
          bool 是否开镜 = 读写.getPtr64(开镜地址) != 0;  // 假设非0表示开镜
        if (是否开镜){
        // 计算主要角度     	    
	    if (准心最近) {	    	
	       Angles angles222 = CalculateViewAngle(自身,准心最近坐标);     
           读写.writefloat(准心地址, angles.pitch);
   	       读写.writefloat(准心地址+0x4, angles222.yaw); 	    	    
	      }	 
	      if (距离最近) {	    	    
            Angles angles222 = CalculateViewAngle(自身, 距离最近坐标);
 	        读写.writefloat(准心地址, angles.pitch);
	        读写.writefloat(准心地址+0x4, angles222.yaw);               	                 
          }
        }
      } else { 
       if (准心最近) {	    	
	       Angles angles222 = CalculateViewAngle(自身,准心最近坐标);     
           读写.writefloat(准心地址, angles.pitch);
   	       读写.writefloat(准心地址+0x4, angles222.yaw); 	    	    
	      }	 
	      if (距离最近) {	    	    
          Angles angles222 = CalculateViewAngle(自身, 距离最近坐标);
 	      读写.writefloat(准心地址, angles.pitch);
	      读写.writefloat(准心地址+0x4, angles222.yaw);               	                 
         }      
       }
     }
   }
 }





  





ImGuiStyle* style;
void BeginDraw_Stayle()
{
    style = &ImGui::GetStyle();	
    ImGui::SetNextWindowBgAlpha(1);
    style->WindowRounding = 1.0f;
    style->LogSliderDeadzone = 2.0f;
    style->WindowTitleAlign = ImVec2(0.5, 0.5);
    style->FramePadding = ImVec2(7, 7);
    
	style->FrameBorderSize = 0;//设置控件描边宽度
    style->FrameRounding = 4.5f;   //圆角
    style->WindowRounding = 4.5f;   //圆角
	
	style->WindowBorderSize = 0.0;//设置框架描边宽度
	style->GrabRounding = 1.0f;   // 滚动条圆角
    style->ScrollbarSize = 30.0f;   // 滚动条圆角
	}
	
void ImGuiMainMenuWind() {
ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::StyleColorsDark(); // 使用内置暗黑主题（背景黑，文字白）
    // 设置为纯黑风格
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    // ... 其他颜色设置 ...
style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // 纯白文字
    // 其他代码 ...




        ImGuiStyle &Style = ImGui::GetStyle();
        io.ConfigWindowsMoveFromTitleBarOnly = false;
        io.WantSaveIniSettings = true; 
		BeginDraw_Stayle();





		
    	static bool show_ChildMenu1 = false;
		static bool show_ChildMenu2 = true;
		static bool show_ChildMenu3 = false;
	    ImGui::Begin("@DevloperCode"); 
		ImGui::SetWindowSize({700, 500}, ImGuiCond_Once); //窗口大小
	    Window = ImGui::GetCurrentWindow();
		 if(ImGui::Button("绘制",ImVec2(167,80)))
            {
                show_ChildMenu1 = false;
                show_ChildMenu2 = true;
                show_ChildMenu3 = false;
            }
            ImGui::SameLine();
            if(ImGui::Button("自瞄",ImVec2(167,80)))
            {
                show_ChildMenu1 = true;
                show_ChildMenu2 = false;
                show_ChildMenu3 = false;
            }
           ImGui::SameLine();
            if(ImGui::Button("功能",ImVec2(167,80)))
            {
                show_ChildMenu1 = false;
                show_ChildMenu2 = false;
                show_ChildMenu3 = true;
            }
           









if(show_ChildMenu2){

ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.8f, 0.1f, 0.1f, 1.0f)); // 按钮背景色（暗红色）
ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.2f, 0.2f, 1.0f)); // 悬停时的颜色
ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.9f, 0.0f, 0.0f, 1.0f)); // 按下时的颜色

	    if (ImGui::Button("[初始化]", ImVec2(200, 50))) 
		
		{
	    按钮.绘制 = true;
	    }
ImGui::PopStyleColor(3); // 恢复默认颜色（弹出3次修改）
		
        ImGui::ItemSize(ImVec2(0, 10));  
			if (ImGui::Button("一键开"))
				{
					//killGG();
					按钮.方框 = true;
					按钮.射线 = true;
					按钮.雷达 = true;
					按钮.距离 = true;
					按钮.血量 = true;
				}
        ImGui::Checkbox("显示方框", &按钮.方框);
        ImGui::SameLine();
        ImGui::Checkbox("显示射线", &按钮.射线);
        ImGui::SameLine();
        ImGui::Checkbox("显示距离", &按钮.距离);
		ImGui::SameLine();
       // ImGui::Checkbox("显示名字", &按钮.名字);
		//ImGui::Checkbox("显示阵营", &按钮.阵营);
		ImGui::SameLine();
        ImGui::Checkbox("显示血量", &按钮.血量);
        ImGui::SameLine();
        ImGui::Checkbox("显示雷达", &按钮.雷达);
    /*    ImGui::Text("便利 如果漏人往上面调(越低越流畅 越高越卡)"); 
        ImGui::SliderFloat("绘制遍历数量", &配置.绘制便利, 5000,20000);*/
		/*ImGui::SameLine();
        ImGui::Checkbox("子弹落点", &按钮.子弹落点);*/


        /*
        ImGui::Text("以下为绘制物品"); 
		ImGui::Checkbox("人", &按钮.人);
		ImGui::SameLine();
		ImGui::Checkbox("熊", &按钮.熊);
		ImGui::SameLine();
        ImGui::Checkbox("鹿", &按钮.鹿);
		ImGui::SameLine();
		ImGui::Checkbox("猪", &按钮.猪);
		*/
		ImGui::Text("雷达配置"); 
		ImGui::SliderFloat("雷达X", &配置.雷达X, 0, 4000);
        ImGui::SliderFloat("雷达Y", &配置.雷达Y, 0, 2000);
        ImGui::Text("绘制距离"); 
		//ImGui::SliderFloat("绘制最大距离", &配置.绘制最大距离, 0, 250,"%f",1);
		
		
		
        ImGui::Text("模块头: %p ",libbase); 
        
    }
	
if(show_ChildMenu1){
    ImGui::Checkbox("开启自瞄", &按钮.自瞄);
    ImGui::Checkbox("开镜判断", &按钮.开镜);
	ImGui::Checkbox("准星优先", &准心最近);
	ImGui::Checkbox("距离优先", &距离最近);
	ImGui::SliderFloat("自瞄上下调节", &配置.自瞄位置, -7, 7);
	//ImGui::SliderFloat("自瞄圆圈范围", &配置.自瞄范围, 0, 1000);
	ImGui::SliderFloat("自瞄距离", &配置.自瞄距离, 0, 200);
    
    

    }

if(show_ChildMenu3){
ImGui::Text("如果夜市没效果重新进入服务器就有效果了"); 
ImGui::Text("(确保开着夜市退出服务器在进入)"); 

ImGui::Checkbox("人物夜市", &按钮.夜市开关);
    if (按钮.夜市开关) {
uintptr_t 夜市 = 读写.getPtr64(读写.getPtr64(读写.getPtr64(读写.getPtr64(libbase +0x7FA58)+0x508)+0x2A8)+0x380)+0x50;
float 修改 = 10;
读写.writefloat(夜市, 修改); // 
}else{
uintptr_t 夜市 = 读写.getPtr64(读写.getPtr64(读写.getPtr64(读写.getPtr64(libbase +0x7FA58)+0x508)+0x2A8)+0x380)+0x50;
float 修改 = 0.5;
读写.writefloat(夜市, 修改); // 
}

ImGui::Checkbox("黑天空", &按钮.黑色天空);
    if (按钮.黑色天空) {
 uintptr_t 黑天 = 读写.getPtr64(读写.getPtr64(读写.getPtr64(读写.getPtr64(libbase +0x1C160)+0xA0)+0x30)+0x2D0)+0x3C;
float 修改 = 999;
读写.writefloat(黑天, 修改); // 
//0x1C160+0xA0+0x30+0x2D0+0x3C
}else{
uintptr_t 黑天 = 读写.getPtr64(读写.getPtr64(读写.getPtr64(读写.getPtr64(libbase +0x1C160)+0xA0)+0x30)+0x2D0)+0x3C;
float 修改 = 0.95;
读写.writefloat(黑天, 修改);
}

ImGui::Checkbox("房屋透视", &按钮.房屋透视);
    if (按钮.房屋透视) {
 uintptr_t 透视 = 读写.getPtr64(读写.getPtr64(读写.getPtr64(libbase + 0x60618)+0x20)+0x20)+0x464;
float 修改 = 6;
读写.writefloat(透视, 修改); // 
//0x60618+0x20+0x20+0x464
}else{
uintptr_t 透视 = 读写.getPtr64(读写.getPtr64(读写.getPtr64(libbase + 0x60618)+0x20)+0x20)+0x464;
float 修改 = 0.15000000596046448;
读写.writefloat(透视, 修改);
}



}
QingZ(ImGui::GetForegroundDrawList());
}

