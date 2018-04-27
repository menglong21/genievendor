/*****************************************************************
* 阿里巴巴人工智能实验室 厂商开放代码
*
* 仅供与人工智能实验室的合作厂商内部开发使用
* 禁止任何形式的开源或传播
* 
* Author: tongfei.changtf@alibaba-inc.com 
*
*****************************************************************/

/*****************************************************************
* 模块名称
*
* GenieVendor电量监控
*
* 模块功能
*
* 1、启动socket监听和按键事件监听
* 2、头文件定义全局日志开关，定义一些通用宏
*
*****************************************************************/
#define _BATTERY_MONITOR_IMPL_

#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include "batterymonitor.h"
#include "genievendor.h"
#include "logic.h"
#undef LOG_TAG
#define LOG_TAG "GVBatteryMonitor"

/*电量上报间隔*/
#define REPORT_INTERVAL     (120) //sec
/*电量节点*/
#define ADC_FD_PATH         "/dev/ircut"
/*满电电压*/
#define BATTERY_VOLTAGE_MAX 4.2
#define BATTERY_VOLTAGE_MIN 3.3

pthread_mutex_t batterypthreadlock;
pthread_cond_t  batterypthreadruncond;

void* _battery_monitor_thread(void* arg){
    FUNCTION_BEGIN
    int fd, ret, adc_val;//,adc_voltage;
    /* tongfei removed begin */
    /*
    volatile float battery_voltage=0.0;
    volatile int battery_percent=0;
    */
    /* tongfei removed end */
    
    /*tongfei code begin */
    int battery_percent = 0;
    /*tongfei code end */
    
	//usleep(30 * 1000 * 1000);//sleep 30S when booting
    while(1){
            if(LCTX.is_poweron_keypress_lock){
                pthread_mutex_lock(&batterypthreadlock);
                pthread_cond_wait(&batterypthreadruncond,&batterypthreadlock);
                pthread_mutex_unlock(&batterypthreadlock);
            }

        fd = open(ADC_FD_PATH, O_RDONLY);
        if(fd < 0){
            LOGE("BATTERY ERROR: Open ircut failed!");
            goto next;
        }
        
        ret = read(fd, &adc_val, sizeof(adc_val));
        if(ret != sizeof(adc_val)){
            LOGE("BATTERY ERROR: Read ircut Failed");
            goto next;
        }
/* tongfei removed begin */
/*
        else{     
            LOGD("BATTERY ADC Sample value is:%d",adc_val);     
        }
*/
/* tongfei removed end */
        close(fd);
/* tongfei removed begin */
/*
        adc_voltage=(3300*adc_val)/4095;         
        LOGD("BATTERY ADC Sample voltage is:%dmv",adc_voltage);
        battery_voltage=(((float)(adc_voltage))/1000.0f);
        battery_voltage*=2;
        LOGD("Battery voltage is :%.2fv",battery_voltage);
            battery_percent = (int)(((battery_voltage-BATTERY_VOLTAGE_MIN)/(BATTERY_VOLTAGE_MAX-BATTERY_VOLTAGE_MIN)) * 100.0f);    
*/
/* tongfei removed end */

/* tongfei code begin: */
/* 
从上面这些代码看，将adc值转换为电压，操作是：
adc+val * 3300 / 4095 / 1000 * 2
当ADC采集值为2047，则：
2047.5 * 3300 / 4095 / 1000 * 2 = 3.3(V)
当ADC采集2605，则：
2568.7 * 3300 / 4095 / 1000 * 2 = 4.14(V)
所以拿ADC直接跟2047--3.3V时ADC的值 和 2568--4.14V时ADC的值比较，就可以得出百分比了，这里不需要知道实际电压是多少。
5.21 = (2568 - 2047) / 100

如果出现电压不准，先确定是否ADC读不准的问题，有时候ADC值波动很大，需要硬件排查。
*/
        battery_percent = (int)((adc_val - 2047.0f) / 5.21f);

/* tongfei code end */
        if(battery_percent > 100) {        
            battery_percent = 100;
        }
/* tongfei code begin: */
        else if (battery_percent < 1){
            battery_percent = 1;
        }
/* tongfei code end */
        LOGD("Battery adc value: %d, percent: %d%%",adc_val,battery_percent);
        logic_handle_battery_percent(&LCTX, battery_percent);

        next:
        if(fd > 0){
            close(fd);
        }
        usleep(REPORT_INTERVAL * 1000 * 1000);
    }
    
    FUNCTION_END
}

/*启动监控线程*/
int battery_monitor_start(pthread_t* thread){
    FUNCTION_BEGIN
    int ret;
    ret = pthread_create(thread, NULL, _battery_monitor_thread, NULL);
    if(0 == ret){
        LOGD("battery monitor thread created!");
        FUNCTION_END
        return GV_OK;
    } else {
        LOGE("battery monitor thread create failed!");
        FUNCTION_END
        return GV_FAIL;
    }
}


