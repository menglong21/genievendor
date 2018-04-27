/*****************************************************************
*
* 阿里巴巴人工智能实验室 厂商开放代码
*
* 仅供与人工智能实验室的合作厂商内部开发使用
* 禁止任何形式的开源或传播
* 
* Author: tongfei.changtf@alibaba-inc.com 
*
*****************************************************************/

#ifndef _LIGHT_MANAGER_H_
#define _LIGHT_MANAGER_H_

#ifdef __cplusplus  
extern "C" {  
#endif  

#undef EXTERN
#ifdef _LIGHT_MANAGER_IMPL_
#define EXTERN
#else
#define EXTERN extern
#endif

#include "genievendor.h"

typedef unsigned char LED_COLOR_T;

#define LED_COLOR_OFF       ((LED_COLOR_T)0)
#define LED_COLOR_RED       ((LED_COLOR_T)0x4)
#define LED_COLOR_GREEN     ((LED_COLOR_T)0x2)
#define LED_COLOR_BLUE      ((LED_COLOR_T)0x1)
#define LED_COLOR_YELLOW    ((LED_COLOR_T)(LED_COLOR_RED | LED_COLOR_GREEN))
#define LED_COLOR_CYAN      ((LED_COLOR_T)(LED_COLOR_GREEN | LED_COLOR_BLUE))
#define LED_COLOR_MEGENTA   ((LED_COLOR_T)(LED_COLOR_RED | LED_COLOR_BLUE))
#define LED_COLOR_WHITE     ((LED_COLOR_T)(LED_COLOR_RED | LED_COLOR_GREEN | LED_COLOR_BLUE))

typedef struct{
    int red_fd;
    int green_fd;
    int blue_fd;
    char msgid;
    BOOL is_blinking;
    BOOL is_light_in_process;
    LED_COLOR_T light_hold_color;
    BOOL is_light_on_hold;
} LED_CONTEXT_T;

extern LED_CONTEXT_T LED_CTX;
EXTERN int light_set_hold_color(LED_CONTEXT_T* LED_CTX,  LED_COLOR_T hold_color);
EXTERN int light_try_set_hold(LED_CONTEXT_T* LED_CTX, BOOL is_on_hold);
EXTERN int light_on(LED_CONTEXT_T * LED_CTX, LED_COLOR_T color);
EXTERN int light_on_duration(LED_CONTEXT_T * LED_CTX, LED_COLOR_T color, int duration);
EXTERN int light_on_blink_start(LED_CONTEXT_T * LED_CTX, LED_COLOR_T color, int duration);
EXTERN int light_on_blink_stop(LED_CONTEXT_T * LED_CTX);

EXTERN int light_off(LED_CONTEXT_T * LED_CTX);

//灯控初始化
EXTERN int light_manager_start(pthread_t * thread);

#ifdef __cplusplus  
}  
#endif 

#endif /*_LIGHT_MANAGER_H_*/
