/*****************************************************************
*
*
*****************************************************************/

/*****************************************************************
* 模块名称
*
* GenieVendor LED控制模块
*
* 模块功能
*
* 实现LED控制
*
*****************************************************************/


#define _LIGHT_MANAGER_IMPL_

#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <memory.h>
#include "lightmanager.h"
#include "libaglog.h"

#undef LOG_TAG
#define LOG_TAG "GVLightManager"

#define GPIO_FD_RED     "/sys/class/gpio/gpio99/value"
#define GPIO_FD_GREEN   "/sys/class/gpio/gpio132/value"
#define GPIO_FD_BLUE    "/sys/class/gpio/gpio130/value"

#define LED_ON          "1"
#define LED_OFF         "0"

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;


typedef struct led_msg_t{
    char id;
    unsigned char color;
    int duration;
    char is_blink;
    struct led_msg_t * next;
} LED_CTL_MSG_T;

LED_CTL_MSG_T* g_msg_head;
LED_CTL_MSG_T* g_msg_tail;
int g_msg_list_length = 0;

LED_CONTEXT_T LED_CTX = {0};

static int _atom_set_led_color(LED_CONTEXT_T* LED_CTX, LED_COLOR_T color){
    if(color & LED_COLOR_RED){
        write(LED_CTX->red_fd, LED_ON, 1);
    } else {
        write(LED_CTX->red_fd, LED_OFF, 1);
    }
    
    if(color & LED_COLOR_GREEN){
        write(LED_CTX->green_fd, LED_ON, 1);
    } else {
        write(LED_CTX->green_fd, LED_OFF, 1);
    }
    
    if(color & LED_COLOR_BLUE){
        write(LED_CTX->blue_fd, LED_ON, 1);
    } else {
        write(LED_CTX->blue_fd, LED_OFF, 1);
    }

    return GV_OK;
}

static int _append_light_ctl_msg(LED_CONTEXT_T* LED_CTX, LED_COLOR_T color, int duration, char is_blink){
    LED_CTL_MSG_T * msg;

    if(LED_CTX->is_blinking){
        LOGE("LED is blinking! Ignore all the LED controls now.");
        return GV_FAIL;
    }

    //free在msg被处理后
    msg = (LED_CTL_MSG_T *)malloc(sizeof(LED_CTL_MSG_T));
    memset(msg, 0, sizeof(LED_CTL_MSG_T));
    msg->color = color;
    msg->duration = duration;
    msg->is_blink = is_blink;

    if(LED_CTX->msgid == 255){
        LED_CTX->msgid = 0;
    }
    msg->id = LED_CTX->msgid;
    LED_CTX->msgid++;
    LOGD("Appending a light msg, MsgId:[%d] Color:[0x%02X] Duration:[%d]ms IsBlink:[%d]",msg->id, msg->color, 
    msg->duration, (int)msg->is_blink);
    pthread_mutex_lock(&mut);
    if(g_msg_head == NULL){
        g_msg_head = msg;
    } else {
        g_msg_tail->next = msg;
    }
    g_msg_tail = msg;
    g_msg_list_length++;
    LOGD("head id: %d  tail id: %d, g_msg_length: %d", g_msg_head->id, g_msg_tail->id, g_msg_list_length);
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mut);
    return GV_OK;
}

int light_set_hold_color(LED_CONTEXT_T* LED_CTX,  unsigned char hold_color){
    LOGD("Led hold color set to %d", (int)hold_color);
    LED_CTX->light_hold_color = hold_color;
    return GV_OK;
}

int light_try_set_hold(LED_CONTEXT_T* LED_CTX, BOOL is_on_hold){
    if(is_on_hold){
        if(!LED_CTX->is_light_in_process && g_msg_list_length == 0){
            LED_CTX->is_light_on_hold = TRUE;
            LOGD("Led hold is set to ON");
            _append_light_ctl_msg(LED_CTX, LED_CTX->light_hold_color, 0, FALSE);
        } else {
            LOGW("Led msg is in processing or there are more messages followed, ignore this led hold control.");
            return GV_FAIL;
        }
    } else {
        LED_CTX->is_light_on_hold = FALSE;
        _append_light_ctl_msg(LED_CTX, LED_COLOR_OFF, 0, FALSE);
        LOGD("Led hold is set to OFF");
    }
    return GV_OK;
}

int light_on_duration(LED_CONTEXT_T* LED_CTX, LED_COLOR_T color, int duration){
    _append_light_ctl_msg(LED_CTX, color, duration, FALSE);
    _append_light_ctl_msg(LED_CTX, LED_COLOR_OFF, 0, FALSE);
    return GV_OK;
}

int light_on(LED_CONTEXT_T* LED_CTX, LED_COLOR_T color){
    _append_light_ctl_msg(LED_CTX, color, 0, FALSE);
    return GV_OK;
}

int light_off(LED_CONTEXT_T* LED_CTX){
    _append_light_ctl_msg(LED_CTX, LED_COLOR_OFF, 0, FALSE);
    return GV_OK;
}

int light_on_blink_start(LED_CONTEXT_T* LED_CTX, LED_COLOR_T color, int duration){
    FUNCTION_BEGIN
    _append_light_ctl_msg(LED_CTX, color, duration, TRUE);
    FUNCTION_END
    return GV_OK;
}

int light_on_blink_stop(LED_CONTEXT_T* LED_CTX){
    FUNCTION_BEGIN
    if(!LED_CTX->is_blinking){
        LOGW("LED is NOT blinking right now!!!");
        return GV_FAIL;
    }
    LED_CTX->is_blinking = FALSE;
    //force turn off light
    light_off(LED_CTX);
    FUNCTION_END
    return GV_OK;
}

void* _light_controller_thread(void* arg){

    LED_CTL_MSG_T * msg;
    LED_COLOR_T color[2];
    color[1] = LED_COLOR_OFF;
    int color_index = 0;
    
    while(1){
        pthread_mutex_lock(&mut);
        while (NULL == g_msg_head) {
            pthread_cond_wait(&cond, &mut);
        }
        msg = g_msg_head;
        g_msg_head = g_msg_head->next;
        g_msg_list_length--;        
        LOGD("Processing a light msg, MsgId:[%d] Color:[0x%02X] Duration:[%d]ms Blink:[%d] MsgListLenAfterProcess:[%d]", msg->id, msg->color, 
                msg->duration, msg->is_blink, g_msg_list_length);
        pthread_mutex_unlock(&mut);
        LED_CTX.is_light_in_process = TRUE;
        LED_CTX.is_blinking = msg->is_blink;
        color[0] = msg->color;
        color_index = 0;
        do{
            _atom_set_led_color(&LED_CTX, color[color_index]);
            if(msg->duration > 0){
                usleep(msg->duration * 1000);
            }
            color_index = 1 - color_index;
        } while(LED_CTX.is_blinking);
        
        free(msg);
        LED_CTX.is_light_in_process = FALSE;
        if(LED_CTX.is_light_on_hold && LED_COLOR_OFF == color[0]){
            _atom_set_led_color(&LED_CTX, LED_CTX.light_hold_color);
        }
    }
}


int light_manager_start(pthread_t * thread){
    LED_CTX.red_fd = open(GPIO_FD_RED, O_WRONLY);
    LED_CTX.green_fd = open(GPIO_FD_GREEN, O_WRONLY);
    LED_CTX.blue_fd = open(GPIO_FD_BLUE, O_WRONLY);
    
    if(LED_CTX.red_fd < 0 || LED_CTX.green_fd < 0 || LED_CTX.blue_fd < 0){
        LOGE("Cannot open LED GPIO for write!!");
        return GV_FAIL;
    }
    
    int ret = pthread_create(thread, NULL, _light_controller_thread, NULL);
    if(ret != 0){
        return GV_FAIL;
    }
	return GV_OK;
}


