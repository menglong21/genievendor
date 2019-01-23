/*****************************************************************
*
* 
*****************************************************************/

#ifndef _KEY_MANAGER_H_
#define _KEY_MANAGER_H_

#ifdef __cplusplus  
extern "C" {  
#endif  


#undef EXTERN
#ifdef _KEY_MANAGER_IMPL_
#define EXTERN
#else
#define EXTERN extern
#endif

#include <linux/input.h>

#define KEY_ALLOW_HANDLE_INTERVAL   ((long)500) //ms
#define KEY_ALLOW_HANDLE_VOICE_KEY_INTERVAL   ((long)1000) //ms

#define KEYEVENT_COND_TIMEOUT_INTERVAL  11 //11s
/*
 * keyevent status type, up/down/longpress
 */
#define KEYVALUE_LONG_PRESS       2
#define KEYVALUE_KEY_DOWN         1
#define KEYVALUE_KEY_UP           0

/*
 * hardware keycdoe definition
 */
#define HWKEY_VOL_UP        1
#define HWKEY_VOL_DOWN      2
#define HWKEY_LED           3
#define HWKEY_PREV          4
#define HWKEY_PLAYPAUSE     5
#define HWKEY_NEXT          6
#define HWKEY_TRANSLATE     7
#define HWKEY_FAVORITES     8
#define HWKEY_SINOLOGY      9
#define HWKEY_STORY         10
#define HWKEY_MUSIC         11
#define HWKEY_AI            12
#define HWKEY_INTERCOM      13
#define HWKEY_RESET			14
#define HWKEY_CHARGE        15
#define HWKEY_INVALID       255

/*
 * genievendor keycode definition
 * parse from thich key and key up/down/longpress status
 */
typedef enum {
    GVKEY_INVAILD = 0,
    GVKEY_WIFI_SETUP,
    GVKEY_PREV,
    GVKEY_NEXT,
    GVKEY_PLAYPAUSE,
    GVKEY_VOLUME_UP,
    GVKEY_VOLUME_DOWN,
    GVKEY_PLAY_MUSIC,
    GVKEY_PLAY_SINOLOGY,
    GVKEY_PLAY_STORY,
	GVKEY_PLAY_ENGLISH,
    GVKEY_AI_START,
    GVKEY_AI_STOP,
    GVKEY_TRANSLATE_START,
    GVKEY_TRANSLATE_STOP,
    GVKEY_INTERCOM_START,
    GVKEY_INTERCOM_STOP,
    GVKEY_OPERATE_FAVORITE,
    GVKEY_PLAY_FAVORITE,
    GVKEY_PLAY_INTERCOM,
    GVKEY_LED,
    GVKEY_RESET,
    GVKEY_CHARGING,
    GVKEY_UNCHARGING,
} AG_KEYCODE_E;

EXTERN void keyevent_callback(struct input_event* event);
EXTERN int keyevent_check(struct input_event* event);
EXTERN int keymanager_start();

#ifdef __cplusplus
}
#endif

#endif /*_KEY_MANAGER_H_*/
