/*****************************************************************
*
*
*****************************************************************/

/*****************************************************************
* 模块名称
*
* GenieVendor逻辑处理
*
* 模块功能
*
* 1、同步当前整机状态
* 2、处理按键事件
* 3、处理GenieEngine发来的消息
* 4、灯效的逻辑控制
* 5、其他逻辑控制
*
*****************************************************************/


#define _LOGIC_IMPL_

#include <pthread.h>
#include <signal.h>

#include "logic.h"
#include "batterymonitor.h"
#include "usbmonitor.h"
#include "libaglog.h"
#include "keyevent_reader.h"

#undef LOG_TAG
#define LOG_TAG "GVLogic"


LOGIC_CONTEXT_T LCTX = {0};

static int _send_handshake_to_engine(){
    FUNCTION_BEGIN
    char send_buffer[VENDOR_CMD_LENGTH_MAX] = {0};
    json_pack_handshake(send_buffer);
    socket_send(&SCTX, send_buffer, strlen(send_buffer));
    FUNCTION_END
    return GV_OK;
}

static int _send_childlock_status_to_engine(const char * status){
    FUNCTION_BEGIN
    char send_buffer[VENDOR_CMD_LENGTH_MAX] = {0};
    json_pack_childlock_status(send_buffer, status);
    socket_send(&SCTX, send_buffer, strlen(send_buffer));
    FUNCTION_END
    return GV_OK;

}

static int _send_player_resume_to_engine(){
    FUNCTION_BEGIN
    char send_buffer[VENDOR_CMD_LENGTH_MAX] = {0};
    json_pack_player_resume(send_buffer);
    socket_send(&SCTX, send_buffer, strlen(send_buffer));
    FUNCTION_END
    return GV_OK;
}

static int _send_player_stop_to_engine(){
    FUNCTION_BEGIN
    char send_buffer[VENDOR_CMD_LENGTH_MAX] = {0};
    json_pack_player_stop(send_buffer);
    socket_send(&SCTX, send_buffer, strlen(send_buffer));
    FUNCTION_END
    return GV_OK;
}

static int _send_player_pause_to_engine(){
    FUNCTION_BEGIN
    char send_buffer[VENDOR_CMD_LENGTH_MAX] = {0};
    json_pack_player_pause(send_buffer);
    socket_send(&SCTX, send_buffer, strlen(send_buffer));
    FUNCTION_END
    return GV_OK;
}

static int _send_media_play_to_engine(const char * path){
    FUNCTION_BEGIN
    char send_buffer[VENDOR_CMD_LENGTH_MAX] = {0};
    json_pack_media_play(send_buffer, path);
    socket_send(&SCTX, send_buffer, strlen(send_buffer));
    FUNCTION_END
    return GV_OK;
}
static int _send_prompt_play_to_engine(const char *path){ 
    FUNCTION_BEGIN
    char send_buffer[VENDOR_CMD_LENGTH_MAX] = {0};
    json_pack_prompt_play(send_buffer, path);
    socket_send(&SCTX, send_buffer, strlen(send_buffer));
    FUNCTION_END
    return GV_OK;    
}

static int _send_volume_adjust_key_to_engine(const char * keyname){
    FUNCTION_BEGIN
    char send_buffer[VENDOR_CMD_LENGTH_MAX] = {0};
    json_pack_volume_adjust(send_buffer, keyname, VOLUME_STEP_LENGTH);
    socket_send(&SCTX, send_buffer, strlen(send_buffer));
    FUNCTION_END
    return GV_OK;
}

static int _send_key_to_engine(const char * keyname){
    FUNCTION_BEGIN

    char send_buffer[VENDOR_CMD_LENGTH_MAX+1] = {0};
    json_pack_keyevent(send_buffer, keyname);
    socket_send(&SCTX, send_buffer, strlen(send_buffer));
    FUNCTION_END
    return GV_OK;
}

static int _send_battery_percent_status_to_engine(const int percent){
    FUNCTION_BEGIN
    char send_buffer[VENDOR_CMD_LENGTH_MAX+1] = {0};
    json_pack_battery_status(send_buffer, percent);
    socket_send(&SCTX, send_buffer, strlen(send_buffer));
    FUNCTION_END
    return GV_OK;
}

static int _send_led_status_to_engine(const char * isLightOn){
    FUNCTION_BEGIN
    char send_buffer[VENDOR_CMD_LENGTH_MAX+1] = {0};
    json_pack_led_status(send_buffer, isLightOn);
    socket_send(&SCTX, send_buffer, strlen(send_buffer));
    FUNCTION_END
    return GV_OK;
}

static int tone_add_or_del_status(FAV_RETVALUE_E retValue)
{
	 switch (retValue) {
            case ADD_SUCCESS:
                 _send_prompt_play_to_engine(TONE_ADD_FAV_SUC);
                 break;
            case ADD_FAIL:
                 _send_prompt_play_to_engine(TONE_ADD_FAV_FAIL);
                 break;
	    case DEL_SUCCESS:
				 _send_player_stop_to_engine();
                 _send_prompt_play_to_engine(TONE_CANCEL_FAV_SUC);
                 break;
	    case DEL_FAIL:
                 _send_prompt_play_to_engine(TONE_CANCEL_FAV_FAIL);
                 break;
	    case ADD_EMPTY:
				 _send_player_stop_to_engine();
                 _send_prompt_play_to_engine(TONE_NO_FAV);
                 break;
            default:
             break;
    }
  return GV_OK;
}
static int _handle_keyevent_online(AG_KEYCODE_E keycode){
    
    switch(keycode){
        case GVKEY_PREV:
            light_on_duration(&LED_CTX, LED_COLOR_BLUE, LED_DURATION_KEY_PRESS_BLINK);
            _send_key_to_engine(G_PARAMS_VALUE_BUTTON_NAME_PREV);
            break;
        case GVKEY_NEXT:
            light_on_duration(&LED_CTX, LED_COLOR_CYAN, LED_DURATION_KEY_PRESS_BLINK);
            _send_key_to_engine(G_PARAMS_VALUE_BUTTON_NAME_NEXT);
            break;
        case GVKEY_AI_START:
            light_on(&LED_CTX, LED_COLOR_RED);
            LCTX.player_status_ctx.player_current_opcode.eventcode=HWKEY_AI;
            LCTX.player_status_ctx.player_current_opcode.keycode=GVKEY_AI_START;
            LCTX.is_running_keypress_lock=TRUE;
            _send_key_to_engine(G_PARAMS_VALUE_BUTTON_NAME_AI_START);
            break;
        case GVKEY_AI_STOP:
            light_off(&LED_CTX);
            _send_key_to_engine(G_PARAMS_VALUE_BUTTON_NAME_AI_STOP);
            break;
        case GVKEY_PLAYPAUSE:
            light_on_duration(&LED_CTX, LED_COLOR_MEGENTA, LED_DURATION_KEY_PRESS_BLINK);
            _send_key_to_engine(G_PARAMS_VALUE_BUTTON_NAME_PLAYPAUSE);
            break;
        case GVKEY_PLAY_MUSIC:
            light_on_duration(&LED_CTX, LED_COLOR_RED, LED_DURATION_KEY_PRESS_BLINK);
			_send_player_stop_to_engine();//stop all first
            _send_key_to_engine(G_PARAMS_VALUE_BUTTON_NAME_PLAY_MUSIC);        
            break;
        case GVKEY_PLAY_SINOLOGY:
            light_on_duration(&LED_CTX, LED_COLOR_CYAN, LED_DURATION_KEY_PRESS_BLINK);
			_send_player_stop_to_engine();//stop all first
            _send_key_to_engine(G_PARAMS_VALUE_BUTTON_NAME_PLAY_SINOLOGY);
            break;
         case GVKEY_PLAY_ENGLISH:
			_send_player_stop_to_engine();//stop all first
			//_send_prompt_play_to_engine(PROMPT_FILE_ENGLISH);//tf music
            _send_key_to_engine(G_PARAMS_VALUE_BUTTON_NAME_PLAY_ENGLISH);
            break;
        case GVKEY_PLAY_STORY:
            light_on_duration(&LED_CTX, LED_COLOR_BLUE, LED_DURATION_KEY_PRESS_BLINK);
			_send_player_stop_to_engine();//stop all first
            _send_key_to_engine(G_PARAMS_VALUE_BUTTON_NAME_PLAY_STORY);
            break;
        case GVKEY_TRANSLATE_START:
            light_on(&LED_CTX, LED_COLOR_RED);
            _send_key_to_engine(G_PARAMS_VALUE_BUTTON_NAME_TRANSLATE_START);
            break;
        case GVKEY_TRANSLATE_STOP:
            light_off(&LED_CTX);
            _send_key_to_engine(G_PARAMS_VALUE_BUTTON_NAME_TRANSLATE_STOP);
            break;
        case GVKEY_INTERCOM_START:
            light_on(&LED_CTX, LED_COLOR_RED);
            LCTX.player_status_ctx.player_current_opcode.eventcode=HWKEY_INTERCOM;
            LCTX.player_status_ctx.player_current_opcode.keycode=GVKEY_INTERCOM_START;
            LCTX.is_running_keypress_lock=TRUE;
            _send_key_to_engine(G_PARAMS_VALUE_BUTTON_NAME_INTERCOM_START);
            break;
        case GVKEY_INTERCOM_STOP:
            light_off(&LED_CTX);
            _send_key_to_engine(G_PARAMS_VALUE_BUTTON_NAME_INTERCOM_STOP);
            break;
        case GVKEY_PLAY_INTERCOM:
            if(LED_CTX.is_blinking && LCTX.is_new_voice_msg_notifing){
                LCTX.is_new_voice_msg_notifing = FALSE;
                light_on_blink_stop(&LED_CTX);
            }
            light_on_duration(&LED_CTX, LED_COLOR_RED, LED_DURATION_KEY_PRESS_BLINK);
            _send_key_to_engine(G_PARAMS_VALUE_BUTTON_NAME_PLAY_INTERCOM);
            break;
        case GVKEY_OPERATE_FAVORITE:
            light_on_duration(&LED_CTX, LED_COLOR_GREEN, LED_DURATION_KEY_PRESS_BLINK);
            _send_key_to_engine(G_PARAMS_VALUE_BUTTON_NAME_ADD_FAVORITE);
            break;
        case GVKEY_PLAY_FAVORITE:
            light_on_duration(&LED_CTX, LED_COLOR_GREEN, LED_DURATION_KEY_PRESS_BLINK);
			_send_player_stop_to_engine();//stop all first
            _send_key_to_engine(G_PARAMS_VALUE_BUTTON_NAME_PLAY_FAVORITE);
            break;
        default:
            return GV_FAIL;
    }
    return GV_OK;

}

static int _handle_keyevent_offline(AG_KEYCODE_E keycode){
    char * media_path = NULL;
    int ret=0;
    switch(keycode){
    case GVKEY_PREV:
        light_on_duration(&LED_CTX, LED_COLOR_BLUE, LED_DURATION_KEY_PRESS_BLINK);
        

		get_media_path_previous(&media_path);
		LOGD("prev media_path: %08x", media_path);
        if(NULL == media_path){
			if (get_player_media_type() == M_TYPE_FAVOURITE) {
				_send_prompt_play_to_engine(TONE_NO_FAV);
			}
            LOGE("Get next media path failed!");
            goto fail;
        }
        LCTX.player_status_ctx.player_playmode=GV_PLAYER_PLAY_MEDIAKEY;
        _send_media_play_to_engine(media_path);
        break;        
    case GVKEY_NEXT:
        light_on_duration(&LED_CTX, LED_COLOR_CYAN, LED_DURATION_KEY_PRESS_BLINK);
        
       
        get_media_path_next(&media_path);
		LOGD("next media_path: %08x", media_path);
        if(NULL == media_path){
			if (get_player_media_type() == M_TYPE_FAVOURITE) {
				_send_prompt_play_to_engine(TONE_NO_FAV);
			}
            LOGE("Get next media path failed!");
            goto fail;
        }
        LCTX.player_status_ctx.player_playmode=GV_PLAYER_PLAY_MEDIAKEY;
        _send_media_play_to_engine(media_path);
        break;
    case GVKEY_AI_START:
		_send_prompt_play_to_engine(TONE_PLEASE_CONNECT_NETWORK);
        light_on(&LED_CTX, LED_COLOR_RED);
        
        break;
    case GVKEY_AI_STOP:
        light_off(&LED_CTX);
        
        break;
    case GVKEY_PLAYPAUSE:
        light_on_duration(&LED_CTX, LED_COLOR_MEGENTA, LED_DURATION_KEY_PRESS_BLINK);
		LOGD("LCTX.player_status_ctx.player_status=%d", LCTX.player_status_ctx.player_status);
        if(GV_PLAYER_PLAY == LCTX.player_status_ctx.player_status){
            LOGD("player playing, pause it.");
            _send_player_pause_to_engine();
        } else if (GV_PLAYER_PAUSE == LCTX.player_status_ctx.player_status){
            LOGD("player paused, resume it.");
            _send_player_resume_to_engine();
        } else {
            LOGW("No playing or paused media right now.");
        }
        break;
    case GVKEY_PLAY_MUSIC:
        
         if((get_player_media_type() == M_TYPE_SONG) && (GV_PLAYER_PLAY == LCTX.player_status_ctx.player_status))
           break;
         light_on_duration(&LED_CTX, LED_COLOR_RED, LED_DURATION_KEY_PRESS_BLINK);
		 if (LCTX.is_music_empty)
		 {
		 	break;
		 }
		_send_player_stop_to_engine();//stop all first   
        get_media_path_by_type(&media_path, M_TYPE_SONG);
        LOGD("media_path: %08x", media_path);
        if(NULL == media_path){
            LOGE("Get music media path failed!");
            goto fail;
         }else{
            _send_prompt_play_to_engine(TONE_FILE_MUSIC);   
        }
         LCTX.player_status_ctx.player_playmode=GV_PLAYER_PLAY_MEDIAKEY;
        _send_media_play_to_engine(media_path);
        break;
    case GVKEY_PLAY_SINOLOGY:
        
         if((get_player_media_type() == M_TYPE_SINOLOGY) && (GV_PLAYER_PLAY == LCTX.player_status_ctx.player_status))
            break;
         light_on_duration(&LED_CTX, LED_COLOR_CYAN, LED_DURATION_KEY_PRESS_BLINK);
		 if (LCTX.is_sinology_empty)
		 {
		 	break;
		 }
		_send_player_stop_to_engine();//stop all first	
        get_media_path_by_type(&media_path, M_TYPE_SINOLOGY);
        LOGD("media_path: %08x", media_path);
        if(NULL == media_path){
            LOGE("Get sinology media path failed!");
            goto fail;
         }else{
            _send_prompt_play_to_engine(TONE_FILE_SINOLOGY);            
        }
         LCTX.player_status_ctx.player_playmode=GV_PLAYER_PLAY_MEDIAKEY;            
        _send_media_play_to_engine(media_path);
        break;
    case GVKEY_PLAY_STORY:
        
        if((get_player_media_type() == M_TYPE_STORY) && (GV_PLAYER_PLAY == LCTX.player_status_ctx.player_status))
            break;
         light_on_duration(&LED_CTX, LED_COLOR_BLUE, LED_DURATION_KEY_PRESS_BLINK);
		 if (LCTX.is_sinology_empty)
		 {
		 	break;
		 }
		_send_player_stop_to_engine();//stop all first	
        get_media_path_by_type(&media_path, M_TYPE_STORY);
        LOGD("media_path: %08x", media_path);
        if(NULL == media_path){
            LOGE("Get story media path failed!");
            goto fail;
         }else{
            _send_prompt_play_to_engine(TONE_FILE_STORY);
        }
         LCTX.player_status_ctx.player_playmode=GV_PLAYER_PLAY_MEDIAKEY;
        _send_media_play_to_engine(media_path);
        break;
    case GVKEY_PLAY_ENGLISH:
		_send_player_stop_to_engine();//stop all first
		 //_send_prompt_play_to_engine(PROMPT_FILE_ENGLISH);//tf music
        _send_media_play_to_engine(media_path);
        break;		
    case GVKEY_TRANSLATE_START:
		_send_prompt_play_to_engine(TONE_PLEASE_CONNECT_NETWORK);
        light_on(&LED_CTX, LED_COLOR_RED);
        
        break;
    case GVKEY_TRANSLATE_STOP:
        light_off(&LED_CTX);
        
        break;
    case GVKEY_INTERCOM_START:
		_send_prompt_play_to_engine(TONE_PLEASE_CONNECT_NETWORK);
        light_on(&LED_CTX, LED_COLOR_RED);
        
        break;
    case GVKEY_INTERCOM_STOP:
        light_off(&LED_CTX);
        
        break;
    case GVKEY_OPERATE_FAVORITE:
        light_on_duration(&LED_CTX, LED_COLOR_GREEN, LED_DURATION_KEY_PRESS_BLINK);
        ret=operate_mediapath_favourite();		
	tone_add_or_del_status(ret);
        break;
    case GVKEY_PLAY_FAVORITE:
        light_on_duration(&LED_CTX, LED_COLOR_GREEN, LED_DURATION_KEY_PRESS_BLINK);
        if((operate_is_favouriteFile_empty() == GV_FAIL) || (operate_is_favouriteFile_empty() == GV_ERR))
        {
           //tone favourite
           _send_prompt_play_to_engine(TONE_NO_FAV);
           break;
        }
        if((get_player_media_type() == M_TYPE_FAVOURITE) && (GV_PLAYER_PLAY == LCTX.player_status_ctx.player_status))
            break;
        
		_send_player_stop_to_engine();//stop all first
        LCTX.player_status_ctx.player_playmode=GV_PLAYER_PLAY_MEDIAKEY;
        get_media_path_by_type(&media_path, M_TYPE_FAVOURITE);
        if(NULL == media_path){
            LOGE("Get favourite media path failed!");
            goto fail;
        }
        _send_media_play_to_engine(media_path);
        break;
    case GVKEY_PLAY_INTERCOM:
        if(LED_CTX.is_blinking && LCTX.is_new_voice_msg_notifing){
            LCTX.is_new_voice_msg_notifing = FALSE;
            light_on_blink_stop(&LED_CTX);
        }
        _send_prompt_play_to_engine(TONE_PLEASE_CONNECT_NETWORK);
        light_on_duration(&LED_CTX, LED_COLOR_RED, LED_DURATION_KEY_PRESS_BLINK);
        break;
    default:
        return GV_FAIL;
    }
    return GV_OK;

    fail:
        return GV_FAIL;
}


int logic_handle_keyevent(LOGIC_CONTEXT_T * CTX, AG_KEYCODE_E keycode){

    FUNCTION_BEGIN
    int ret = GV_OK;

    //判断童锁状态
    if(CTX->is_childlock_on){
        LOGD("Childlock is ON, ignore all keyevent.");
        return ret;
    }

    //不区分在线离线的按键处理
    switch(keycode){
        case GVKEY_CHARGING:
             LCTX.player_usb_status_ctx.cable_status=GV_CABLE_PLUGIN;
             logic_usb_pulginkey_detect();
             if(get_sdcard_scanstatus()!=GV_SDCARD_UMOUNT){
                LCTX.player_usb_status_ctx.cable_status=GV_CABLE_CHARGING;
                logic_handle_charge();     
             }
             break;
        case GVKEY_UNCHARGING:
             LCTX.player_usb_status_ctx.cable_status=GV_CABLE_PLUGOUT;
             if(get_sdcard_scanstatus()!=GV_SDCARD_UMOUNT){
                LCTX.player_usb_status_ctx.cable_status=GV_CABLE_UNCHARGING; 
                logic_handle_charge();
             }
             break;
		case GVKEY_RESET:
            if ((LCTX.player_usb_status_ctx.cable_status==GV_CABLE_PLUGOUT) || (LCTX.player_usb_status_ctx.cable_status == GV_CABLE_STANDBY)) {
				logic_handle_factory_reset();
			}
			break;
        case GVKEY_WIFI_SETUP:
            if ((LCTX.player_usb_status_ctx.cable_status==GV_CABLE_PLUGOUT) || (LCTX.player_usb_status_ctx.cable_status == GV_CABLE_STANDBY)) {
				light_on_blink_start(&LED_CTX, LED_COLOR_RED, LED_DURATION_INTERVAL_WIFI_SETUP_BLINK);
				LCTX.player_status_ctx.player_playmode=GV_PLAYER_PLAY_HOLD;    
				_send_key_to_engine(G_PARAMS_VALUE_BUTTON_NAME_WIFI_SETUP);
			}
            break;
        case GVKEY_LED:
            if ((LCTX.player_usb_status_ctx.cable_status==GV_CABLE_PLUGOUT) || (LCTX.player_usb_status_ctx.cable_status == GV_CABLE_STANDBY)) {
				light_set_hold_color(&LED_CTX, LED_COLOR_YELLOW);
				if(GV_OK == light_try_set_hold(&LED_CTX, !LED_CTX.is_light_on_hold)){
					if(LED_CTX.is_light_on_hold){
						_send_led_status_to_engine(G_PARAMS_VALUE_LED_STATUS_ON);
					} else {
						_send_led_status_to_engine(G_PARAMS_VALUE_LED_STATUS_OFF);
					}
				}
			}
            break;
		case GVKEY_VOLUME_UP:
            if ((LCTX.player_usb_status_ctx.cable_status==GV_CABLE_PLUGOUT) || (LCTX.player_usb_status_ctx.cable_status == GV_CABLE_STANDBY)) {
				light_on_duration(&LED_CTX, LED_COLOR_GREEN, LED_DURATION_KEY_PRESS_BLINK);
				LCTX.player_status_ctx.player_playmode=GV_PLAYER_PLAY_HOLD;
				_send_volume_adjust_key_to_engine(G_PARAMS_VALUE_BUTTON_NAME_VOLUME_UP);
			}
            break;
        case GVKEY_VOLUME_DOWN:
            if ((LCTX.player_usb_status_ctx.cable_status==GV_CABLE_PLUGOUT) || (LCTX.player_usb_status_ctx.cable_status == GV_CABLE_STANDBY)) {
				light_on_duration(&LED_CTX, LED_COLOR_RED, LED_DURATION_KEY_PRESS_BLINK);
				LCTX.player_status_ctx.player_playmode=GV_PLAYER_PLAY_HOLD;
				_send_volume_adjust_key_to_engine(G_PARAMS_VALUE_BUTTON_NAME_VOLUME_DOWN);
			}
            break;
        default:
            if ((LCTX.player_usb_status_ctx.cable_status==GV_CABLE_PLUGOUT) || (LCTX.player_usb_status_ctx.cable_status == GV_CABLE_STANDBY)) {
				if(GV_NETWORK_CONNECTED == CTX->network_status){
					//在线按键处理
					LOGD("process online keycode");
					ret = _handle_keyevent_online(keycode);
				} else {
					//离线按键处理
					LOGD("process offline keycode");
					ret = _handle_keyevent_offline(keycode);
				}
			}
            break;
    }
    
    if(GV_OK != ret){
        LOGE("Unhandled keycode: %d", keycode);
        FUNCTION_END
        return GV_FAIL;
    }
    FUNCTION_END
    return GV_OK;
}

int logic_handle_battery_percent(LOGIC_CONTEXT_T * CTX, int percent){

    CTX->battery_percent = percent;
    FUNCTION_BEGIN





    if(CTX->is_handshake_with_engine_done){
        _send_battery_percent_status_to_engine(percent);
    }


    if(percent<20){
        light_on_duration(&LED_CTX, LED_COLOR_RED, LED_DURATION_BATTERY_ENERGY_LOW_BLINK);  
        if(percent<20&&percent>=10){
            LOGD("notify battery percent is %%%d,low than %20",percent);
            _send_prompt_play_to_engine(TONE_BATTERY_NEED_CHARGE);
        }else if(percent<10&&percent>=5){
            LOGD("notify battery percent is %%%d,low than %10",percent);
            _send_prompt_play_to_engine(TONE_BATTERY_NEED_CHARGE); 
        }else if(percent<5&&percent>=0){
            LOGD("notify battery percnet is %%%d,low than 5%");
            _send_prompt_play_to_engine(TONE_BATTTERY_ENERGEY_NOT_ENOUGH);
            system("poweroff");
        }
    }   
    FUNCTION_END
    return GV_OK;
}

int logic_handle_player_status(LOGIC_CONTEXT_T * CTX, char* name, char* author, PLAYER_STATUS_E status){

    strncpy(CTX->media_author, author, sizeof(CTX->media_author));
    strncpy(CTX->media_name, name, sizeof(CTX->media_name));
    (CTX->player_status_ctx).player_status = status;
    LOGD("Player status updated: name:[%s] author:[%s] status:[%d]", name, author, status);
    if((CTX->player_status_ctx).player_playmode==GV_PLAYER_PLAY_AUTO){
        if(status==GV_PLAYER_STOP&&(CTX->network_status== GV_NETWORK_DISCNNECTED||CTX->network_status==GV_NETWORK_UNKNOWN)){
            char * media_path = NULL;                   
            	get_media_path_next(&media_path);    
            if(media_path){
            	_send_media_play_to_engine(media_path);
            }
        }
    }
    return GV_OK;
}

int logic_handle_network_status(LOGIC_CONTEXT_T * CTX, NETWORK_STATUS_E status){

    CTX->network_status = status;
    LOGD("Network status updated: status:[%d]", status);
    if(GV_NETWORK_CONNECTED == status){
        light_on_blink_stop(&LED_CTX);
        light_on_duration(&LED_CTX, LED_COLOR_GREEN, LED_DURATION_WIFI_CONNECTED);
    }
    //else打开，则断网后红灯闪
    /* else {
        light_on_blink_start(&LED_CTX, LED_COLOR_RED, LED_DURATION_INTERVAL_WIFI_SETUP_BLINK);
    }*/
    return GV_OK;
}

int logic_handle_usb_status_change(LOGIC_CONTEXT_T *CTX,USB_STATUS_E status){       
    CTX->player_usb_status_ctx.usb_status=status;
    if((CTX->player_status_ctx).player_status == GV_PLAYER_PLAY)
        _send_player_pause_to_engine();
    return GV_OK;
}

int logic_handle_sd_status_change(LOGIC_CONTEXT_T *CTX,SDCARD_STATUS_E status){
    CTX->player_sd_status_ctx.sd_status=status;

    return GV_OK;
}

int logic_handle_system_status(LOGIC_CONTEXT_T * CTX, SYSTEM_STATUS_E status){
    CTX->system_status = status;
    return GV_OK;
}

int logic_handle_cloud_ready(){
    //send battery status
    if(LCTX.is_poweron_keypress_lock==FALSE)
    	_send_battery_percent_status_to_engine(LCTX.battery_percent);
    //tell phone app childlock has beed disabled
    _send_childlock_status_to_engine(G_PARAMS_VALUE_CHILDLOCK_STATUS_OFF);
    //send LED state
    if(LED_CTX.is_light_on_hold){
        _send_led_status_to_engine(G_PARAMS_VALUE_LED_STATUS_ON);
    }
    LCTX.is_poweron_keypress_lock=FALSE;    
    LCTX.is_running_keypress_lock=FALSE;
    pthread_cond_signal(&batterypthreadruncond);
    pthread_cond_signal(&usbpthreadruncond); 
    return GV_OK;
}

int logic_handle_childlock_control(int isOn){
    LCTX.is_childlock_on = isOn != 0;
    if(LCTX.is_childlock_on){
        _send_prompt_play_to_engine("/home/prompt/childlock_on.mp3");
    } else {
        _send_prompt_play_to_engine("/home/prompt/childlock_off.mp3");
    }
    _send_childlock_status_to_engine(isOn? G_PARAMS_VALUE_CHILDLOCK_STATUS_ON: G_PARAMS_VALUE_CHILDLOCK_STATUS_OFF);
    return GV_OK;
}

int logic_handle_led_control(int isOn){
    light_set_hold_color(&LED_CTX, LED_COLOR_YELLOW);
    if(GV_OK == light_try_set_hold(&LED_CTX, 1 == isOn)){
        if(LED_CTX.is_light_on_hold){
            _send_led_status_to_engine(G_PARAMS_VALUE_LED_STATUS_ON);
        } else {
            _send_led_status_to_engine(G_PARAMS_VALUE_LED_STATUS_OFF);
        }
    }
    return GV_OK;
}

int logic_handle_config_reset(){
    //清除配网
    LOGD("removing network configuration...");
    system("rm /mnt/user/d45d98e820c9048846336f10dfbbeaf87031f7be");

    //清除音量设置
	LOGD("set volume to default...");
	system("tinymix 22 35");
    
    //清除休眠设置
    LOGD("Cancelling hibernation...");
    system("rm -f /mnt/user/hibernatetime");
    LCTX.hibernation_delay_sec = 0;
    alarm(0);

    //TODO 清空本地收藏
    system("rm -f /mnt/sdcard/favourite.dat");
    return GV_OK;
}

int logic_handle_hibernationTime_save(const int delaySec)
{
   FILE *fp = NULL;
   fp = fopen("/mnt/user/hibernatetime","w+");
   if(fp == NULL)
   {
      LOGD("open /mnt/user/hibernatetime file failed");
      return GV_FAIL;
   }
   fprintf(fp,"%d", delaySec);
   fclose(fp);
   return GV_OK;
}

int logic_handle_hibernationTime_read()
{
   int hiberTime; 
   FILE *fp = NULL;
   fp = fopen("/mnt/user/hibernatetime", "r");
   if(fp == NULL)
   {
     LOGD("open /mnt/user/hibernatetime file failed");
     return GV_FAIL;
   }
   fscanf(fp,"%d",&hiberTime);
   LOGD("read hibernatetime value is %d.", hiberTime);
   fclose(fp);
   return hiberTime;
}

int logic_handle_hibernationTime_start_read()
{
   int delayTime = logic_handle_hibernationTime_read();
   logic_handle_hibernation_control(delayTime);
   return GV_OK;
}

int logic_handle_hibernation_control(int delaySec){
    LCTX.hibernation_delay_sec = delaySec;
    alarm(delaySec);
    return GV_OK;
}

int logic_handle_genie_speak_led_control(){
    LCTX.is_genie_speak_processing = TRUE;
    LOGD("Genie-speak processing, LED blink start.");
    light_on_blink_start(&LED_CTX, LED_COLOR_YELLOW, LED_DURATION_INTERVAL_GENIE_SPEAK_BLINK);
    return GV_OK;
}

int logic_handle_play_tts_done(){
    if(LCTX.system_status==GV_SYSTEM_BOOTING){
        logic_handle_system_status(&LCTX,GV_SYSTEM_NORMAL);
        pthread_cond_signal(&keypthreadruncond);
    }else{
    if(LCTX.is_genie_speak_processing){
        LOGD("Genie-speak processing finished, LED blink stop.");
        light_on_blink_stop(&LED_CTX);
        LCTX.is_genie_speak_processing = FALSE;
        }
        if((LCTX.player_status_ctx.player_current_opcode.eventcode==HWKEY_INTERCOM)||(LCTX.player_status_ctx.player_current_opcode.eventcode==HWKEY_AI)){
                LCTX.is_running_keypress_lock=FALSE;
                LCTX.player_status_ctx.player_current_opcode.eventcode=HWKEY_INVALID;
                LCTX.player_status_ctx.player_current_opcode.keycode=GVKEY_INVAILD;
        }
    }
    return GV_OK;
}

int logic_handle_play_prompt_done(){
    if(LCTX.system_status==GV_SYSTEM_BOOTING){
        logic_handle_system_status(&LCTX,GV_SYSTEM_NORMAL);        
        pthread_cond_signal(&keypthreadruncond);         
    }else{
        if((LCTX.player_status_ctx.player_current_opcode.eventcode==HWKEY_INTERCOM)||(LCTX.player_status_ctx.player_current_opcode.eventcode==HWKEY_AI)){
                LCTX.is_running_keypress_lock=FALSE;
                LCTX.player_status_ctx.player_current_opcode.eventcode=HWKEY_INVALID;
                LCTX.player_status_ctx.player_current_opcode.keycode=GVKEY_INVAILD;
        }
    }
    return GV_OK;
}

int logic_handle_wifi_setup_result(){
    light_on_blink_stop(&LED_CTX);
    return GV_OK;
}

int logic_handle_factory_reset(){
	system("/usr/sbin/factory_reset.sh");
    return GV_OK;
}

int logic_handle_new_voice_message(){
    if(!LED_CTX.is_blinking){
        LCTX.is_new_voice_msg_notifing = TRUE;
        light_on_blink_start(&LED_CTX, LED_COLOR_BLUE, LED_DURATION_NEW_VOICE_MESSAGE);
    }
    return GV_OK;
}

int logic_handle_voice_message_played(){
    if(LED_CTX.is_blinking && LCTX.is_new_voice_msg_notifing){
        LCTX.is_new_voice_msg_notifing = FALSE;
        light_on_blink_stop(&LED_CTX);
    }
    return GV_OK;
}

int logic_handle_charge(){
    if(LCTX.player_usb_status_ctx.cable_status == GV_CABLE_CHARGING){
        if(LCTX.player_status_ctx.player_status == GV_PLAYER_PLAY){
                LCTX.player_status_ctx.player_playmode=GV_PLAYER_PLAY_HOLD;            
                _send_player_stop_to_engine();
        }
            LOGD("device is charging,please don't use");
            _send_prompt_play_to_engine(TONE_BATTERY_CHARING);   
            usleep(3 * 1000 * 1000);
        	
            system("ifconfig wlan0 down");         
            system("echo 134 > /sys/class/gpio/export"); 
            system("echo out > /sys/class/gpio/gpio134/direction");
            system("echo 0 > /sys/class/gpio/gpio134/value");        
            system("echo 0 > /sys/class/gpio/gpio117/value");
     }else if(LCTX.player_usb_status_ctx.cable_status==GV_CABLE_UNCHARGING){
        system("reboot");                
    }

    return GV_OK;
}
int logic_usb_pulginkey_detect(void){
    int ret = GV_OK;
    int timewait;
    struct timespec ts;
    ts.tv_sec = (time_t)time(0)+4;
    ts.tv_nsec = 0;
    pthread_mutex_lock(&keypthreadlock);
    timewait = pthread_cond_timedwait(&keypthreadruncond, &keypthreadlock, &ts);
    if(timewait==0){
        ret=GV_OK;
    }else if(timewait == ETIMEDOUT){
        ret=GV_FAIL;
    }        
    pthread_mutex_unlock(&keypthreadlock);
    return ret;
}
void* _logic_looper(void* arg){
    
    //这行打开，则开机后红灯闪
    //handle_network_status(&LCTX, GV_NETWORK_UNKNOWN);

    while(!LCTX.is_handshake_with_engine_done){
        LOGD("send handshake to engine...");
        _send_handshake_to_engine();
        usleep(1 * 1000 * 1000);
    }
    LOGD("vendor handshake with engine DONE!");
    while(1){
        usleep(60 * 1000 * 1000);
    }
}

static void _do_hibernation(int signo, siginfo_t *info, void *context){
    //停止播放
    if(GV_PLAYER_PLAY == LCTX.player_status_ctx.player_status){
        _send_player_pause_to_engine();
    }
    
    //关闭LED
    logic_handle_led_control(0);
}

int logic_init(pthread_t * thread){
    int ret;

    //注册ALERT信号监听, 休眠用
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = _do_hibernation;
    sigaction(SIGALRM, &sa, NULL);

    ret = pthread_create(thread, NULL, _logic_looper, NULL);
    return ret;
}
