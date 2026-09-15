/*#include <ctime>
#include <dirent.h>
#include <exception>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <malloc.h>
#include <pthread.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/system_properties.h>
#include <unistd.h>

#include "Encrypt.h"
#include "cJSON.h"
#include "cJSON.c"
#include "http.h"
using namespace std;

char *ztm;
char *jhm;
char *jqm;
char *vip;
char *rc4key = OBFUSCATE("xkrY8Rbm6kh2Ec6");//选择rc4-2加密
char *host = OBFUSCATE("wy.llua.cn");//官网
char *dmdl = OBFUSCATE("api/?id=kmlogon&app=29044");    // 9999改你的应用id
char *imei_luj = OBFUSCATE("/sdcard/tc/imei");
char *km_luj = OBFUSCATE("/sdcard/tc/km");
int 状态码 = 445;

char buffer[20];

char *wydl(const char *km , const char *imei ) {
       
    // 获取时间
    time_t t;
    t = time(NULL);
    int ii = time(&t);

    // 合并微验提交数据
    char value[256];
    char sign[256];
    char data[256];
    sprintf(value, "%d", ii); 
    sprintf(data, "kami=%s&markcode=%s&t=%d&value=%s", km, imei, ii,value);
    char* adga = Encrypt(data, rc4key);    
    char url[128];
    sprintf(url, "&data=%s", adga);
    ztm = httppost(host, dmdl, url);
    char* abcdstr = Decrypt(ztm, rc4key);    
    cJSON *cjson = cJSON_Parse(abcdstr);
    int code = cJSON_GetObjectItem(cjson, "code")->valueint;
    int time = cJSON_GetObjectItem(cjson, "time")->valueint;
    char *msg = cJSON_GetObjectItem(cjson, "msg")->valuestring;

    char *加密 = Encrypt(std::to_string(状态码).c_str(), rc4key);
    char *解密 = Decrypt(加密, rc4key);
    
    if (code == std::stoi(解密)) { //状态码
    cJSON* msgJson = cJSON_GetObjectItem(cjson, "msg");
    const char *kmtype = cJSON_GetObjectItem(msgJson, "kmtype")->valuestring; 
    if (strstr(kmtype,"免费")){
    sprintf(buffer,"%s","免费"); 
    return "success";
    } else {
    int vip1 = cJSON_GetObjectItem(msgJson, "vip")->valueint;
    std::time_t timestamp_t = static_cast<std::time_t>(vip1);
    std::tm* timeinfo = std::localtime(&timestamp_t);
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    return "success";
    }
}
    return msg;
}

*/
#include <ctime>
#include <dirent.h>
#include <exception>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <malloc.h>
#include <pthread.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/system_properties.h>
#include <unistd.h>


#include "Encrypt.h"
#include "cJSON.h"
#include "cJSON.c"
#include "http.h"
using namespace std;

char *ztm;
char *jhm;
char *jqm;
char *vip;
char *rc4key = "UDRyvkAIpINgVzb"; //选择rc4-2加密
char *host = "wy.llua.cn";        //官网
char *dmdl = "api/?id=kmlogon&app=49787"; // 9999改你的应用id
char *imei_luj = "/sdcard/tc/imei";
char *km_luj = "/sdcard/tc/km";
char * 状态码 = "441";
char *gg = "api/?id=notice&app=49787";//公告
char *ks = "api/?id=ini&app=49787";//更新接口
char *hgg;
char *更新;
char 公告[1024];
char 更新公告[1024];
char 更新地址[1024];
bool 强制更新 = true;
bool 加载系统界面 = true;

char *版本号 ="0.1";

char buffer[20];

//异步处理网络请求

void wydlAsync(const std::string& km, const std::string& imei) {

    // 使用线程或异步任务处理网络请求

    // ...

}
bool 更新检测(){
char url[1024];
//sprintf(url, "&data=%s", adga);
更新 = httppost(host, ks, "");
char *abcdstrgg = Decrypt(更新, rc4key);
cJSON *cjsongg = cJSON_Parse(abcdstrgg);
int code = cJSON_GetObjectItem(cjsongg, "code")->valueint;
if (code == 200) //状态码
   {
   cJSON *msgJsongg = cJSON_GetObjectItem(cjsongg, "msg");
   const char *version = cJSON_GetObjectItem(msgJsongg, "version")->valuestring;//获取更新版本号
   const char *app_update_show = cJSON_GetObjectItem(msgJsongg, "app_update_show")->valuestring;//更新内容
   const char *appupdatemust = cJSON_GetObjectItem(msgJsongg, "app_update_must")->valuestring;//强制更新
   sprintf(更新公告, "%s", app_update_show);
   if (!strstr(version,版本号)) {
   if (strstr(appupdatemust,"n")) {
     return false;
    }
   if (strstr(appupdatemust,"y")) {
      return true;                      
       }
     }else{
     return false;
     }
   }
}

char *wydl(const char *km, const char *imei)
{   
   

    // 获取时间
    time_t t;
    t = time(NULL);
    int ii = time(&t);

    // 合并微验提交数据
    char value[1024];
    char sign[1024];
    char data[1024];
    sprintf(value, "%d", ii);
    sprintf(data, "kami=%s&markcode=%s&t=%d&value=%s", km, imei, ii, value);
    char *adga = Encrypt(data, rc4key);
    char url[1024];
    sprintf(url, "&data=%s", adga);
    ztm = httppost(host, dmdl, url);
    char *abcdstr = Decrypt(ztm, rc4key);
    cJSON *cjson = cJSON_Parse(abcdstr);
    int code = cJSON_GetObjectItem(cjson, "code")->valueint;
    int time = cJSON_GetObjectItem(cjson, "time")->valueint;
    char *msg = cJSON_GetObjectItem(cjson, "msg")->valuestring;
    
    char *加密 = Encrypt(状态码, rc4key);
    char *解密 = Decrypt(加密, rc4key);
           
    if (code == std::stoi(解密)) //状态码
    {
        cJSON *msgJson = cJSON_GetObjectItem(cjson, "msg");
        const char *kmtype = cJSON_GetObjectItem(msgJson, "kmtype")->valuestring;
        if (strstr(kmtype, "免费"))
        {
            sprintf(buffer, "%s", "免费");
            return "success";
        }
        else
        {
            int vip1 = cJSON_GetObjectItem(msgJson, "vip")->valueint;
            std::time_t timestamp_t = static_cast<std::time_t>(vip1);
            std::tm *timeinfo = std::localtime(&timestamp_t);
            std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
            return "success";
        }
    }else{
          // 处理错误情况
         return "error";
     }
    return msg;
}