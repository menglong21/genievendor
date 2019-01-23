/*****************************************************************
*
*
*****************************************************************/

/*****************************************************************
* 模块名称
*
* GenieVendor按键事件监听与翻译
*
* 模块功能
*
* 1、调用按键监听实现
* 2、将监听到的事件翻译为实际键值(音量加减、播放暂停等)
* 3、调用logic模块处理键值
*
*****************************************************************/


#define _KEY_MANAGER_IMPL_

#include <string.h>
#include <errno.h>

#include "genievendor.h"
#include "keymanager.h"
#include "keyevent_reader.h"
#include "logic.h"
#include "libaglog.h"
#include "usbmonitor.h"
#undef LOG_TAG
#define LOG_TAG "GVKeyManager"

static int g_long_press = 0;
static long g_last_handle_timestamp = 0;
static int g_voice_started = 0;

/************
* LCTX.category_index: swtich song\story\sinology\english
* value :
*   0 : play song
*   1:  play story
*   2:  play sinology
*   3:  play english 
*   others: default to 0[song]
*************/
typedef enum {
    AI_DIRECT_SONG = 0,
    AI_DIRECT_STORY,/*1*/
    AI_DIRECT_SINOLOGY,/*2*/
    AI_DIRECT_ENGLISH,/*3*/
    AI_DIRECT_UNKNOW,/*4*/
} AG_AI_DIRECT_TYPE;

static void reset_ai_short_press_status( void ){
    if(LCTX.category_index >= AI_DIRECT_UNKNOW 
        || LCTX.category_index < AI_DIRECT_SONG){
            LCTX.category_index = 0;
    }
}

/*
 * parse keyevent by type key_down
 */
static int _parse_key_down(int code)
{
    FUNCTION_BEGIN

    int keycode = GVKEY_INVAILD;

    switch (code) {
    case HWKEY_AI:
        keycode = GVKEY_AI_START;
        break;
    case HWKEY_TRANSLATE:
        keycode = GVKEY_TRANSLATE_START;
        break;
    case HWKEY_CHARGE:
        keycode =GVKEY_UNCHARGING;
        break;
    default:
        break;
    }
    FUNCTION_END

    return keycode;
}

/*
 * parse keyevent by type key_longpress
 */
static int _parse_key_longpress(int code)
{
    FUNCTION_BEGIN

    int keycode = GVKEY_INVAILD;

    switch (code) {
    case HWKEY_PLAYPAUSE:
        keycode = GVKEY_WIFI_SETUP;
        break;
    case HWKEY_FAVORITES:
        keycode = GVKEY_OPERATE_FAVORITE;
        break;
    case HWKEY_INTERCOM:
        keycode = GVKEY_INTERCOM_START;
        break;
	case HWKEY_RESET:
		keycode = GVKEY_RESET;
        break;
    case HWKEY_CHARGE:
         if(LCTX.player_usb_status_ctx.cable_status==GV_CABLE_PLUGIN){
             keycode = GVKEY_UNCHARGING;
         }else{
            keycode=GVKEY_INVAILD;
         }
        break;
    default:
        break;
    }
    FUNCTION_END
    return keycode;
}

/*
 * parse keyevent by type key_up
 */
static int _parse_key_up(int code)
{
    FUNCTION_BEGIN

    int keycode = GVKEY_INVAILD;

    switch (code) {
    case HWKEY_AI:
        keycode = GVKEY_AI_STOP;
        LCTX.is_running_keypress_lock=FALSE;
        break;
    case HWKEY_TRANSLATE:
        keycode = GVKEY_TRANSLATE_STOP;
        break;
    case HWKEY_INTERCOM:
        if(g_long_press){
            keycode = GVKEY_INTERCOM_STOP;
            LCTX.is_running_keypress_lock=FALSE;
        } else {
            keycode = GVKEY_PLAY_INTERCOM;
        }
        break;
    
    case HWKEY_PLAYPAUSE:
        if(g_long_press){
            keycode = GVKEY_INVAILD;
        } else {
            keycode = GVKEY_PLAYPAUSE;
        }
        break;

    case HWKEY_NEXT:
        keycode = GVKEY_NEXT;
        break;
    case HWKEY_PREV:
        keycode = GVKEY_PREV;
        break;
    case HWKEY_VOL_UP:
        keycode = GVKEY_VOLUME_UP;
        break;
    case HWKEY_VOL_DOWN:
        keycode = GVKEY_VOLUME_DOWN;
        break;
    case HWKEY_MUSIC:
        keycode = GVKEY_PLAY_MUSIC;
        break;
    case HWKEY_SINOLOGY:
        keycode = GVKEY_PLAY_SINOLOGY;
        break;
    case HWKEY_STORY:
        keycode = GVKEY_PLAY_STORY;
        break;
    case HWKEY_FAVORITES:
        if(g_long_press){
            keycode = GVKEY_INVAILD;
        } else {
            keycode = GVKEY_PLAY_FAVORITE;
        }
        break;
    case HWKEY_LED:
        keycode = GVKEY_LED;
        break;
    case HWKEY_CHARGE:
        keycode = GVKEY_CHARGING;
        break;
    default:
        break;
    }
    FUNCTION_END
    return keycode;
}


static int _parse_keycode(struct input_event* event){
    FUNCTION_BEGIN
    int keycode = GVKEY_INVAILD;
    switch(event->value) {
        case KEYVALUE_KEY_DOWN:
            keycode = _parse_key_down(event->code);
            break;
        case KEYVALUE_LONG_PRESS:
            keycode = _parse_key_longpress(event->code);
            g_long_press = 1;
            break;
        case KEYVALUE_KEY_UP:
            keycode = _parse_key_up(event->code);
            g_long_press = 0;
            break;
        default:
            break;
    }
    return keycode;
    FUNCTION_END

}

void keyevent_callback(struct input_event* event){

    int keycode = GVKEY_INVAILD;
    struct timespec current_timespec;
    long current_timestamp;

    if(NULL == event) {
        LOGE("keyevent callback received a NULL event!");
        return;
    }
    //reset hibernate time
    logic_handle_hibernationTime_start_read();
 
    keycode = _parse_keycode(event);
    LOGD("parsed keycode=%d", keycode);
    if(LCTX.player_usb_status_ctx.cable_status==GV_CABLE_CHARGING){ 
        if(keycode==GVKEY_UNCHARGING){
            LOGD("keycode is %d:device is charging,please don't use",keycode);
            LCTX.is_running_keypress_lock=FALSE;
        }else{
            LCTX.is_running_keypress_lock=TRUE;
        }    
    }
    if(LCTX.is_running_keypress_lock){
        LOGD("KeyPress is Lock,ignore all keyevent.");
        return;
    }   
    switch(keycode){
        case GVKEY_AI_START:
        case GVKEY_TRANSLATE_START:
            clock_gettime(CLOCK_MONOTONIC, &current_timespec);
            current_timestamp = current_timespec.tv_sec * 1000 + 
                                current_timespec.tv_nsec / 1000 / 1000;
            if(current_timestamp - g_last_handle_timestamp > KEY_ALLOW_HANDLE_VOICE_KEY_INTERVAL){
                g_last_handle_timestamp = current_timestamp;
                LOGD("dispatch a voice keycode[%d] at kernel time[%ld]", 
                                keycode, 
                                g_last_handle_timestamp);
               
                logic_handle_keyevent(&LCTX, keycode);
                g_voice_started = 1;
            } else {
                LOGW("Voice key too often, dropped voice keycode:[%d]", keycode);
            }
            break;
        case GVKEY_AI_STOP:
        case GVKEY_TRANSLATE_STOP:
            if(g_voice_started){
                g_voice_started = 0;
                logic_handle_keyevent(&LCTX, keycode);
            } else {
                LOGD("Drop a voice stop key because of not started.[%d]", keycode);
            }
            break;
        case GVKEY_INTERCOM_START:
        case GVKEY_INTERCOM_STOP:
        case GVKEY_VOLUME_UP:
        case GVKEY_VOLUME_DOWN:
            LOGD("dispatch voice and volume related keycode directly.");
            
             
            logic_handle_keyevent(&LCTX, keycode);
            break;
        case GVKEY_INVAILD:
            return;
            
        default:
            clock_gettime(CLOCK_MONOTONIC, &current_timespec);
            current_timestamp = current_timespec.tv_sec * 1000 + 
                                current_timespec.tv_nsec / 1000 / 1000;
            if(current_timestamp - g_last_handle_timestamp > KEY_ALLOW_HANDLE_INTERVAL){
                g_last_handle_timestamp = current_timestamp;
                LOGD("dispatch a non-voice keycode[%d] at kernel time[%ld]", 
                                keycode, 
                                g_last_handle_timestamp);
                logic_handle_keyevent(&LCTX, keycode);
            } else {
                LOGW("KeyEvent too often, dropped keycode:[%d]", keycode);
            }
            break;
    }
}
int keyevent_check(struct input_event* event){
    int ret = GV_OK;
    if((LCTX.player_status_ctx.player_current_opcode.eventcode==HWKEY_INTERCOM)||(LCTX.player_status_ctx.player_current_opcode.eventcode==HWKEY_AI)){
        if(event->code==LCTX.player_status_ctx.player_current_opcode.eventcode){ 
            if(event->value!=KEYVALUE_KEY_UP){
                ret=GV_FAIL;
            }
        }else{
            ret=GV_FAIL;
        }
    }
    return ret;
}

int keymanager_start(pthread_t* thread){
    FUNCTION_BEGIN
    int ret = GV_OK;
    if(GV_OK != keyevent_reader_init(thread)){
        LOGE("keyevent_init failed!");
        ret = GV_FAIL;
    } else {
        LOGD("Keymanager started.");
    }
    FUNCTION_END
    return ret;
}

