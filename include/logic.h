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

#ifndef _LOGIC_H_
#define _LOGIC_H_

#ifdef __cplusplus  
extern "C" {  
#endif  

#undef EXTERN
#ifdef _LOGIC_IMPL_
#define EXTERN
#else
#define EXTERN extern
#endif

#include "genievendor.h"
#include "keymanager.h"
#include "lightmanager.h"
#include "socketmanager.h"
#include "jsonparser.h"
#include "mediamanager.h"

#define LED_DURATION_INTERVAL_WIFI_SETUP_BLINK      (500) //ms
#define LED_DURATION_KEY_PRESS_BLINK                (300) //ms
#define LED_DURATION_WIFI_CONNECTED                 (2000) //ms
#define LED_DURATION_NEW_VOICE_MESSAGE              (500) //ms
#define LED_DURATION_BATTERY_ENERGY_LOW_BLINK       (1000) //ms

#define LED_DURATION_INTERVAL_GENIE_SPEAK_BLINK     (500) //ms

#define MEDIA_NAME_LENGTH_MAX                       (40)
#define MEDIA_AUTHOR_LENGTH_MAX                     (40)

typedef enum{
    GV_USB_DISCONNECTED=0,
    GV_USB_CONNECTED,
}USB_STATUS_E;
    
typedef enum{
    GV_SDCARD_SCAN_NOTSTART=0,
    GV_SDCARD_SCAN_FAILED,
    GV_SDCARD_SCAN_ING,
    GV_SDCARD_SCAN_OK,
    GV_SDCARD_UMOUNT,	
}SDCARD_STATUS_E;


typedef enum{
    GV_CABLE_STANDBY=0,
    GV_CABLE_PLUGIN,
    GV_CABLE_CHARGING,
    GV_CABLE_PLUGOUT,
    GV_CABLE_UNCHARGING,
}CHARGE_STATUS_E;

typedef enum{
    GV_NETWORK_UNKNOWN = 0,
    GV_NETWORK_CONNECTED,
    GV_NETWORK_DISCNNECTED,
} NETWORK_STATUS_E;

typedef enum{
    GV_SYSTEM_UNKNOWN = 0,
    GV_SYSTEM_STANDBY,
    GV_SYSTEM_BOOTING,
    GV_SYSTEM_NORMAL,
} SYSTEM_STATUS_E;

typedef enum{
    GV_PLAYER_UNKNOWN = 0,
    GV_PLAYER_STOP,
    GV_PLAYER_PLAY,
    GV_PLAYER_PAUSE,
} PLAYER_STATUS_E;


typedef enum{
    GV_PLAYER_PLAY_AUTO=0,
    GV_PLAYER_PLAY_MEDIAKEY,
    GV_PLAYER_PLAY_HOLD,    
}PLAYER_PLAY_MODE_E;

typedef struct PLAYER_KEYCODE_CURRENT_CONTEXT{
    int eventcode;    
    int keycode;
}PLAYER_KEYCODE_CURRENT_CONTEX_T;

typedef struct PLAYER_STATUS_CONTEXT{
 PLAYER_STATUS_E player_status;           
 PLAYER_PLAY_MODE_E player_playmode;
    MEDIA_TYPE_E player_mediatype;
    PLAYER_KEYCODE_CURRENT_CONTEX_T player_current_opcode;
}PLAYER_STATUS_CONTEX_T;

typedef struct PLAYER_USB_STATUS_CONTEXT{
    USB_STATUS_E usb_status;        
    BOOL UDiskPlugInFlag;
    CHARGE_STATUS_E cable_status;
}PLAYER_USB_STATUS_CONTEX_T;       

typedef struct PLAYER_SD_STATUS_CONTEXT{
    SDCARD_STATUS_E sd_status;
    BOOL SDcardRefreshFlag;
}PLAYER_SD_STATUS_CONTEX_T;     
/*保存逻辑状态上下文*/
typedef struct{
    SYSTEM_STATUS_E system_status;
    NETWORK_STATUS_E network_status;
    //PLAYER_STATUS_E player_status;
    PLAYER_STATUS_CONTEX_T player_status_ctx; 
    char media_name[MEDIA_NAME_LENGTH_MAX];
    char media_author[MEDIA_AUTHOR_LENGTH_MAX];
    BOOL is_battert_per20_notified;
    BOOL is_battert_per10_notified;
    BOOL is_handshake_with_engine_done;
    int battery_percent;
    BOOL is_childlock_on;
    int hibernation_delay_sec;
    PLAYER_USB_STATUS_CONTEX_T  player_usb_status_ctx; 
    PLAYER_SD_STATUS_CONTEX_T   player_sd_status_ctx;
    BOOL is_genie_speak_processing;
    BOOL is_poweron_keypress_lock;
    BOOL is_running_keypress_lock;
    BOOL is_new_voice_msg_notifing;
	BOOL is_music_empty;
	BOOL is_story_empty;
	BOOL is_sinology_empty;
	BOOL is_favourite_empty;
	int category_index;
} LOGIC_CONTEXT_T;

extern LOGIC_CONTEXT_T LCTX;

EXTERN int logic_handle_keyevent(LOGIC_CONTEXT_T * CTX, AG_KEYCODE_E keycode);
EXTERN int logic_handle_battery_percent(LOGIC_CONTEXT_T * CTX, int percent);
EXTERN int logic_handle_player_status(LOGIC_CONTEXT_T * CTX, char* name, char* author, PLAYER_STATUS_E status);
EXTERN int logic_handle_network_status(LOGIC_CONTEXT_T * CTX, NETWORK_STATUS_E status);
EXTERN int logic_handle_usb_status_change(LOGIC_CONTEXT_T *CTX,USB_STATUS_E status);
EXTERN int logic_handle_sd_status_change(LOGIC_CONTEXT_T *CTX,SDCARD_STATUS_E status);
EXTERN int logic_handle_system_status(LOGIC_CONTEXT_T * CTX, SYSTEM_STATUS_E status);
EXTERN int logic_handle_cloud_ready();
EXTERN int logic_init(pthread_t * thread);
EXTERN int logic_handle_childlock_control(int isOn);
EXTERN int logic_handle_led_control(int isOn);
EXTERN int logic_handle_config_reset();
EXTERN int logic_handle_hibernation_control(int delaySec);
EXTERN int logic_handle_hibernationTime_save(const int delaySec);
EXTERN int logic_handle_hibernationTime_file_read();
EXTERN int logic_handle_hibernationTime_start_read();
EXTERN int logic_handle_genie_speak_led_control();
EXTERN int logic_handle_play_tts_done();
EXTERN int logic_handle_play_prompt_done();
EXTERN int logic_handle_wifi_setup_result();
EXTERN int logic_handle_factory_reset();
EXTERN int logic_handle_new_voice_message();
EXTERN int logic_handle_voice_message_played();
EXTERN int logic_handle_charge(void);
EXTERN int logic_usb_pulginkey_detect(void);

#ifdef __cplusplus  
}  
#endif  

#endif /*_SOCKET_MANAGER_H_*/
