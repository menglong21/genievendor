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


#define _KEYEVENT_READER_IMPL_

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include "genievendor.h"
#include "keymanager.h"
#include "keyevent_reader.h"
#include "logic.h"
#include "batterymonitor.h"
#include "usbmonitor.h"
#include "libaglog.h"
#undef LOG_TAG
#define LOG_TAG "GVKeyEventReader"
#define KEYEVENT_DEVICE_0   "/dev/input/event0"
#define KEYEVENT_DEVICE_1   "/dev/input/event1"
#define TYPE_KEY            (0x01)

#define NUM_KEVENT_FDS 2
static const int KEVENT_READ_SIZE = sizeof(struct input_event);

static int g_kevent_fds[NUM_KEVENT_FDS] = {0};

pthread_mutex_t keypthreadlock;
pthread_cond_t  keypthreadruncond;

void* _keyevent_read_thread(void* arg){
    struct input_event event;
    fd_set readfds;
    int i;
    int ret;
    struct timespec ts;
    ts.tv_sec = (time_t)time(NULL) + KEYEVENT_COND_TIMEOUT_INTERVAL;
    ts.tv_nsec = 0;

    if(LCTX.is_poweron_keypress_lock){
          pthread_mutex_lock(&keypthreadlock);
          
          int retValue =  pthread_cond_timedwait(&keypthreadruncond, &keypthreadlock, &ts);
          if(retValue == ETIMEDOUT)
          {
              LCTX.is_poweron_keypress_lock = FALSE;
              LCTX.is_running_keypress_lock = FALSE;
          }

          pthread_mutex_unlock(&keypthreadlock);
     }
    
    while(1){

        FD_ZERO(&readfds);
        for(i = 0; i < NUM_KEVENT_FDS; i++){
            FD_SET(g_kevent_fds[i], &readfds);
        }
        ret = select(g_kevent_fds[NUM_KEVENT_FDS-1] + 1, &readfds, NULL, NULL, NULL);

        if (ret < 0) {
            //select error
            LOGE("[keyevent_reader] select error, errno=%d", errno);
            goto fail;
        } else if (ret == 0) {
            //timeout or file error
            LOGE("[keyevent_reader] select timeout or bad file descriptor!");
            goto fail;
        }

        for(i = 0; i < NUM_KEVENT_FDS; i++){
            if (FD_ISSET(g_kevent_fds[i], &readfds)) {
                ret = read(g_kevent_fds[i], &event, KEVENT_READ_SIZE);
                if (KEVENT_READ_SIZE != ret){
                    LOGE("[keyevent_reader] read from fd error! errno=%d", errno);
                    goto fail;
                }
                if (EV_KEY != event.type) {
                    goto fail;
                }
                //read successfully.
                LOGD("a EV_KEY keyevent has been read, event.value=%d, event.code=%d", event.value, event.code);
                if(keyevent_check(&event)==GV_FAIL){
                    break;
                }
                keyevent_callback(&event);
                break;
            }
        }
        if(i == NUM_KEVENT_FDS){
            LOGE("Cannot find selected keyevent file descriptor!");
        }
        
        fail:
            continue;
    }
}

int keyevent_reader_init(pthread_t* thread){
    FUNCTION_BEGIN

    int ret;
    
    g_kevent_fds[0] = open(KEYEVENT_DEVICE_0, O_RDONLY);
    if(g_kevent_fds[0] < 0){
        LOGE("[keyevent_reader] open %s error!", KEYEVENT_DEVICE_0);
        goto fail;
    }
    
    g_kevent_fds[1] = open(KEYEVENT_DEVICE_1, O_RDONLY);
        if(g_kevent_fds[1] < 0){
        LOGE("[keyevent_reader] open %s error!", KEYEVENT_DEVICE_1);
        goto fail;
    }

    LOGD("open device(s) success.");
    LOGV("g_kevent_fds[0]=%d, g_kevent_fds[1]=%d", g_kevent_fds[0], g_kevent_fds[1]);
    LCTX.is_poweron_keypress_lock=TRUE;         
    LCTX.is_running_keypress_lock=TRUE;
    ret = pthread_create(thread, NULL, _keyevent_read_thread, NULL);

    if (-1 == ret){
        LOGE("create keyevent reader thread error!");
        LCTX.is_poweron_keypress_lock=FALSE;
        goto fail;
    }
    LOGD("Keyevent reader thread started.");
    
    FUNCTION_END
    return GV_OK;
    
    fail:
        LOGE("[keyevent_init] initialize failed!");
        FUNCTION_END
        return GV_FAIL;
}

