/*****************************************************************
*
*
*****************************************************************/

/*****************************************************************
* GenieVendor与GenieEngine之间JSON API定义
*****************************************************************/

#ifndef _GENIE_VENDOR_API_
#define _GENIE_VENDOR_API_

/*最大method字符长度*/
#define G_JSON_MAX_LENGTH_METHOD                        (30)
/*键名最大长度*/
#define G_JSON_MAX_LENGTH_KEY                           (30)
/*键值最大长度*/
#define G_JSON_MAX_LENGTH_VALUE                         (20)
/*文件路径最大长度*/
#define G_JSON_MAX_PATH_LENGTH                          (256)

/*最大json长度 in byte*/
#define VENDOR_CMD_LENGTH_MAX                           (4096)

/*JSONRPC ROOT KEY-VALUE定义*/
#define G_ROOT_KEY_JSONRPC                              "jsonrpc"
#define G_ROOT_VALUE_JSONRPC_VERSION                    "2.0"
#define G_ROOT_KEY_METHOD                               "method"
#define G_ROOT_KEY_PARAMS                               "params"

/*****************************GenieVendor上报API*****************************/
/*与engine握手*/
#define G_ROOT_VALUE_METHOD_HANDSHAKE                   "handshake"

/*童锁上报*/
#define G_ROOT_VALUE_METHOD_CHILDLOCK                   "childlock"
//params内容同control_childlock

/*按键事件上报*/
#define G_ROOT_VALUE_METHOD_BUTTON                      "button"
#define G_PARAMS_KEY_BUTTON_NAME                        "name"
#define G_PARAMS_KEY_VOLUME_STEP_LENGTH                 "vol_step_len"
/*按键名定义*/
#define G_PARAMS_VALUE_BUTTON_NAME_VOLUME_UP            "volume_inc"
#define G_PARAMS_VALUE_BUTTON_NAME_VOLUME_DOWN          "volume_dec"
#define G_PARAMS_VALUE_BUTTON_NAME_WIFI_SETUP           "wifi_setup"
#define G_PARAMS_VALUE_BUTTON_NAME_PREV                 "play_previous"
#define G_PARAMS_VALUE_BUTTON_NAME_NEXT                 "play_next"
#define G_PARAMS_VALUE_BUTTON_NAME_PLAYPAUSE            "play_pause"
#define G_PARAMS_VALUE_BUTTON_NAME_PLAY_MUSIC           "play_music"
#define G_PARAMS_VALUE_BUTTON_NAME_PLAY_SINOLOGY        "play_sinology"
#define G_PARAMS_VALUE_BUTTON_NAME_PLAY_ENGLISH        "play_english"
#define G_PARAMS_VALUE_BUTTON_NAME_PLAY_STORY           "play_story"
#define G_PARAMS_VALUE_BUTTON_NAME_AI_START             "ai_start"
#define G_PARAMS_VALUE_BUTTON_NAME_AI_STOP              "ai_stop"
#define G_PARAMS_VALUE_BUTTON_NAME_TRANSLATE_START      "translate_start"
#define G_PARAMS_VALUE_BUTTON_NAME_TRANSLATE_STOP       "translate_stop"
#define G_PARAMS_VALUE_BUTTON_NAME_INTERCOM_START       "intercom_start"
#define G_PARAMS_VALUE_BUTTON_NAME_INTERCOM_STOP        "intercom_stop"
#define G_PARAMS_VALUE_BUTTON_NAME_ADD_FAVORITE         "add_favorite"
#define G_PARAMS_VALUE_BUTTON_NAME_PLAY_FAVORITE        "play_favorite"
#define G_PARAMS_VALUE_BUTTON_NAME_PLAY_INTERCOM        "play_intercom"
#define G_PARAMS_VALUE_BUTTON_NAME_LED                  "led"

/*LED事件上报*/
#define G_ROOT_VALUE_METHOD_LED                         "led"
//params同control_lamp

/*电量事件上报*/
#define G_ROOT_VALUE_METHOD_BATTERY                     "battery"
#define G_PARAMS_KEY_BATTERY_PERCENT                    "percent"

/*播放媒体*/
#define G_ROOT_VALUE_METHOD_PLAY_MEDIA                  "play_media"
#define G_PARAMS_KEY_PLAY_MEDIA_PATH                    "path"

/*暂停、继续和停止*/
#define G_ROOT_VALUE_METHOD_PLAYER_PAUSE                "player_pause"
#define G_ROOT_VALUE_METHOD_PLAYER_RESUME               "player_resume"
#define G_ROOT_VALUE_METHOD_PLAYER_STOP               	"player_stop"

/*播放提示音*/
#define G_ROOT_VALUE_METHOD_PLAY_PROMPT                 "play_prompt"
#define G_PARAMS_KEY_PLAY_PROMPT_PATH                   "path"
#define G_PARAMS_KEY_PLAY_PROMPT_VOLUME                 "volume"

/*****************************GenieEngine下发API*****************************/
/*阿里播放器状态下发*/
#define G_ROOT_VALUE_METHOD_PLAYER_STATUS               "player_status_change"
#define G_PARAMS_KEY_AUDIO_NAME                         "audioName"
#define G_PARAMS_KEY_AUDIO_AUTHOR                       "audioAnchor"
#define G_PARAMS_KEY_PLAYER_STATUS                      "status"
#define G_PARAMS_VALUE_PLAYER_STATUS_PLAY               "play"
#define G_PARAMS_VALUE_PLAYER_STATUS_PAUSE              "pause"
#define G_PARAMS_VALUE_PLAYER_STATUS_STOP               "stop"
/*设备联网状态下发*/
#define G_ROOT_VALUE_METHOD_NETWORK_STATUS              "network_status_change"
#define G_PARAMS_KEY_NETWORK_STATUS                     "status"
#define G_PARAMS_VALUE_NETWORK_STATUS_CONNECT           "connect"
#define G_PARAMS_VALUE_NETWORK_STATUS_DISCONNECT        "disconnect"

/*系统状态*/
#define G_ROOT_VALUE_METHOD_SYSTEM_STATUS               "system_status_change"
#define G_PARAMS_KEY_SYSTEM_STATUS                      "status"
#define G_PARAMS_VALUE_SYSTEM_STATUS_STANDBY            "standby"

/*云端准备就绪*/
#define G_ROOT_VALUE_METHOD_CLOUD_READY                 "cloud_ready"

/*LED控制*/
#define G_ROOT_VALUE_METHOD_CONTROL_LED                 "control_led"
#define G_PARAMS_KEY_LED_TARGET                         "targetName"
#define G_PARAMS_KEY_LED_STATUS                         "status"
#define G_PARAMS_VALUE_LED_STATUS_ON                    "on"
#define G_PARAMS_VALUE_LED_STATUS_OFF                   "off"

/*童锁控制*/
#define G_ROOT_VALUE_METHOD_CONTROL_CHILDLOCK           "control_childlock"
#define G_PARAMS_KEY_CHILDLOCK_STATUS                   "status"
#define G_PARAMS_VALUE_CHILDLOCK_STATUS_ON              "on"
#define G_PARAMS_VALUE_CHILDLOCK_STATUS_OFF             "off"

/*还原设置*/
#define G_ROOT_VALUE_METHOD_CONTROL_RESET               "control_reset_config"

/*休眠控制*/
#define G_ROOT_VALUE_METHOD_CONTROL_HIBERNATION         "control_hibernation"
#define G_PARAMS_KEY_HIBERNATION_DELAY                  "delaySeconds"

/*精灵代说消息到达*/
#define G_ROOT_VALUE_METHOD_CONTROL_GENIE_SPEAK         "control_genie_speak"

/*tts状态下发*/
#define G_ROOT_VALUE_METHOD_PLAY_TTS_DONE               "play_tts_done"

/*prompt状态下发*/
#define G_ROOT_VALUE_METHOD_PLAY_PROMPT_DONE            "play_prompt_done"

/*wifi配网结果下发*/
#define G_ROOT_VALUE_METHOD_WIFI_SETUP_RESULT           "wifi_setup_result"
#define G_PARAMS_KEY_WIFI_SETUP_RESULT                  "result"

/*解除配网时恢复出厂*/
#define G_ROOT_VALUE_METHOD_ERASE_USER_DATA             "erase_user_data"

/*收到新留言的通知*/
#define G_ROOT_VALUE_METHOD_NEW_VOICE_MSG               "new_voice_msg"

/*留言播放完毕的通知*/
#define G_ROOT_VALUE_METHOD_VOICE_MSG_PLAYED            "voice_msg_played"

#endif /*_GENIE_VENDOR_API_*/
