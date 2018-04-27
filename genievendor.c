/*****************************************************************
*
* 阿里巴巴人工智能实验室 厂商开放代码
*
* 仅供与人工智能实验室的合作厂商内部开发使用
* 禁止任何形式的开源或传播
* 
* Author: tongfei.changtf@alibaba-inc.com 
* Author: liuwei01@liuwei01@qichengtec.com
          Add:SdCard Scanning Function by 2018.01.08
*****************************************************************/

/*****************************************************************
* 模块名称
*
* GenieVendor程序入口
*
* 模块功能
*
* 1、启动各种监听
* 2、头文件定义全局日志开关，定义一些通用宏
*
*****************************************************************/

#include <assert.h>
#include <pthread.h>
#include <signal.h>

#include "genievendor.h"
#include "socketmanager.h"
#include "keymanager.h"
#include "keyevent_reader.h"
#include "lightmanager.h"
#include "batterymonitor.h"
#include "mediamanager.h"
#include "usbmonitor.h"
#include "logic.h"

#undef LOG_TAG
#define LOG_TAG "GenieVendor"

#define GENIEVENDOR_VERSION "1.0.1"
#define AGLOG_CONFIG_FILE   "/mnt/user/aglog"


static void sighandler(int signo, siginfo_t *info, void *context)
{
    aglog_change_level();
}


int main(int argc, char** argv){

    pthread_t light_thread;
    pthread_t socket_thread;
    pthread_t keymanager_thread;
    pthread_t battery_thread;
	pthread_t media_thread;
	pthread_t usb_thread;
	pthread_t logic_thread;
	
    int err_count = 0;
    struct sigaction sa;

    aglog_read_config(AGLOG_CONFIG_FILE);

    pthread_mutex_init(&sdscanlock, NULL);
    pthread_cond_init(&sdscancond, NULL); 
    pthread_mutex_init(&keypthreadlock,NULL);
    pthread_cond_init(&keypthreadruncond,NULL);
    pthread_mutex_init(&batterypthreadlock,NULL);
    pthread_cond_init(&batterypthreadruncond,NULL);
    pthread_mutex_init(&usbpthreadlock,NULL);
    pthread_cond_init(&usbpthreadruncond,NULL);
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = sighandler;
    sigaction(SIGWINCH, &sa, NULL);

	LOGE("genievendor version : %s",GENIEVENDOR_VERSION);
	
    backtrace_init();

    LOGD("GenieVendor START!");
    logic_handle_system_status(&LCTX,GV_SYSTEM_BOOTING);
    /*启动灯控*/
    LOGD("Starting LightManager...");
    if(GV_OK != light_manager_start(&light_thread)){
        err_count++;
        LOGE("LightManager init failed!");
    }
    
    /*启动socket监听*/
    LOGD("Starting SocketManager...");
    if(GV_OK != socket_start(&socket_thread)){
        err_count++;
        LOGE("SocketManager init failed!");
    }

    /*启动keyevent监听*/
    LOGD("starting keyManager...");
    if(GV_OK != keymanager_start(&keymanager_thread)){
        err_count++;
        LOGE("KeyManager init failed!");
    }

    /*启动battery监听*/
    LOGD("starting battery monitor");
    if(GV_OK != battery_monitor_start(&battery_thread)){
        err_count++;
        LOGE("BatteryMonitor init failed!");
    }

    /*启动T卡媒体扫描*/
    LOGD("Initializing media");
    if(GV_OK != media_manager_init(&media_thread)){
        err_count++;
        LOGE("Media manager init failed!");
    }
	/*启动USBPlug in/out扫描*/
	LOGD("starting usb monitor...");
	if(GV_OK != usb_monitor_start(&usb_thread)){
        err_count++;
        LOGE("USBMonitor init failed!");
    }
	
    /*初始化logic*/
    LOGD("Initializing logic");
    if(GV_OK != logic_init(&logic_thread)){
        err_count++;
        LOGE("logic init failed!");
    }
				
    if(0 == err_count){
        LOGD("GenieVendor init successfully.");
    } else {
        LOGE("GenieVendor init with %d error(s), check E log for more details.", err_count);
    }
    
    pthread_join(light_thread, NULL);
    pthread_join(socket_thread, NULL);
    pthread_join(keymanager_thread, NULL);
    pthread_join(battery_thread, NULL);
	pthread_join(media_thread,NULL);
	pthread_join(usb_thread,NULL);
    pthread_cond_destroy(&sdscancond);
    pthread_mutex_destroy(&sdscanlock);
    pthread_cond_destroy(&keypthreadruncond);
    pthread_mutex_destroy(&keypthreadlock);
    pthread_cond_destroy(&batterypthreadruncond);
    pthread_mutex_destroy(&batterypthreadlock);
    pthread_cond_destroy(&usbpthreadruncond);
    pthread_mutex_destroy(&usbpthreadlock);
    LOGD("genievendor exit");
}

