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

/*****************************************************************
* 模块名称
*
* GenieVendor json打包和解析
*
* 模块功能
*
* 1、解析由GenieEngine发来的json
* 2、构建符合规则的json给GenieEngine
*
*****************************************************************/

#include <string.h>
#include <stdlib.h>
#define _JSON_PARSER_IMPL_
#include "jsonparser.h"
#include "logic.h"

#include "libaglog.h"
#undef LOG_TAG
#define LOG_TAG "GVJsonParser"


/*
函数作用: 根据给定method，打包无param之JSON，生成字符串赋予buf;
buf=
{
    "jsonrpc":  "2.0",
    "method":   "[method]"
}
*/
static int _pack_none_param_json(char * buf, const char * method){
    char * out = NULL;
    cJSON * root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, G_ROOT_KEY_JSONRPC, G_ROOT_VALUE_JSONRPC_VERSION);
    cJSON_AddStringToObject(root, G_ROOT_KEY_METHOD, method);
    out = cJSON_Print(root);
    cJSON_Delete(root);
    strncpy(buf, out, VENDOR_CMD_LENGTH_MAX);
    free(out);
    out = NULL;
    return GV_OK;
}

/*
函数作用: 根据给定method、param_key、param_value, 打包单param之JSON, 生成字符串赋予buf:
buf=
{
    "jsonrpc":  "2.0",
    "method":   [method],
    "params":   {
        [param_key]: [param_value]
    }
}
*/
static int _pack_single_param_json(char* buf,
                        const char* method,
                        const char* param_key,
                        cJSON* param_value){
    char* out = NULL;
    cJSON* root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, G_ROOT_KEY_JSONRPC, G_ROOT_VALUE_JSONRPC_VERSION);
    cJSON_AddStringToObject(root, G_ROOT_KEY_METHOD, method);
    cJSON* params = cJSON_CreateObject();
    cJSON_AddItemToObject(params, param_key, param_value);
    cJSON_AddItemToObject(root, G_ROOT_KEY_PARAMS, params);
    out = cJSON_Print(root);
    cJSON_Delete(root);
    strncpy(buf, out, VENDOR_CMD_LENGTH_MAX);
    free(out);
    out = NULL;
    return GV_OK;
}

static int _pack_double_param_json(char *buf, const char *method, 
                                    const char *key1, cJSON* value1, 
                                    const char *key2, cJSON* value2) {
    char* out = NULL;
    cJSON* root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, G_ROOT_KEY_JSONRPC, G_ROOT_VALUE_JSONRPC_VERSION);
    cJSON_AddStringToObject(root, G_ROOT_KEY_METHOD, method);
    cJSON* params = cJSON_CreateObject();
    cJSON_AddItemToObject(params, key1, value1);
    cJSON_AddItemToObject(params, key2, value2);
    cJSON_AddItemToObject(root, G_ROOT_KEY_PARAMS, params);
    out = cJSON_Print(root);
    cJSON_Delete(root);
    strncpy(buf, out, VENDOR_CMD_LENGTH_MAX);
    free(out);
    out = NULL;
    return GV_OK;
}

int json_pack_handshake(char * buf){
    FUNCTION_BEGIN
    _pack_none_param_json(buf, G_ROOT_VALUE_METHOD_HANDSHAKE);
    FUNCTION_END
    return GV_OK;
}

int json_pack_childlock_status(char *buf, const char* status){
    FUNCTION_BEGIN
    _pack_single_param_json(buf, G_ROOT_VALUE_METHOD_CHILDLOCK, 
                        G_PARAMS_KEY_CHILDLOCK_STATUS, cJSON_CreateString(status));
    FUNCTION_END
    return GV_OK;
}

int json_pack_player_resume(char * buf){
    FUNCTION_BEGIN
    _pack_none_param_json(buf, G_ROOT_VALUE_METHOD_PLAYER_RESUME);
    FUNCTION_END
    return GV_OK;
}

int json_pack_player_stop(char * buf){
    FUNCTION_BEGIN
    _pack_none_param_json(buf, G_ROOT_VALUE_METHOD_PLAYER_STOP);
    FUNCTION_END
    return GV_OK;
}

int json_pack_player_pause(char * buf){
    FUNCTION_BEGIN
    _pack_none_param_json(buf, G_ROOT_VALUE_METHOD_PLAYER_PAUSE);
    FUNCTION_END
    return GV_OK;
}

int json_pack_media_play(char * buf, const char * path){
    FUNCTION_BEGIN
    _pack_single_param_json(buf,
                            G_ROOT_VALUE_METHOD_PLAY_MEDIA,
                            G_PARAMS_KEY_PLAY_MEDIA_PATH,
                            cJSON_CreateString(path));
    FUNCTION_END
    return GV_OK;
}

int json_pack_prompt_play(char *buf,const char *path){
    FUNCTION_BEGIN
    _pack_single_param_json(buf,
                            G_ROOT_VALUE_METHOD_PLAY_PROMPT,
                            G_PARAMS_KEY_PLAY_PROMPT_PATH,
                            cJSON_CreateString(path));
    FUNCTION_END
    return GV_OK;
}

/*构造电量json*/
int json_pack_battery_status(char* buf, const int percent){
    FUNCTION_BEGIN
    /* e.g.
        {
            "jsonrpc":  "2.0",
            "method":   "battery",
            "params":   {
                "value": "20"
            }
        }
    */
    _pack_single_param_json(buf,
                            G_ROOT_VALUE_METHOD_BATTERY,
                            G_PARAMS_KEY_BATTERY_PERCENT,
                            cJSON_CreateNumber(percent));
    FUNCTION_END
    return GV_OK;

}

int json_pack_volume_adjust(char *buf, const char *buttonName, int step_length){
    FUNCTION_BEGIN
    _pack_double_param_json(buf, G_ROOT_VALUE_METHOD_BUTTON, 
                        G_PARAMS_KEY_BUTTON_NAME, cJSON_CreateString(buttonName),
                        G_PARAMS_KEY_VOLUME_STEP_LENGTH, cJSON_CreateNumber((double)step_length));
    FUNCTION_END
    return GV_OK;
}


/*构造按键json*/
int json_pack_keyevent(char* buf, const char* buttonName){
    FUNCTION_BEGIN
/* e.g.

    {
        "jsonrpc":  "2.0",
        "method":   "button",
        "params":   {
            "name": "volume_dec"
        }
    }
*/
    _pack_single_param_json(buf,
                            G_ROOT_VALUE_METHOD_BUTTON,
                            G_PARAMS_KEY_BUTTON_NAME,
                            cJSON_CreateString(buttonName));
    FUNCTION_END
    return GV_OK;

}

int json_pack_led_status(char* buf, const char * status){
    FUNCTION_BEGIN
    /* e.g.

    {
        "jsonrpc":  "2.0",
        "method":   "led",
        "params":   {
            "status": "on"
        }
    }
    */
    _pack_single_param_json(buf,
                            G_ROOT_VALUE_METHOD_LED,
                            G_PARAMS_KEY_LED_STATUS,
                            cJSON_CreateString(status));
    FUNCTION_END
    return GV_OK;
}

static int _json_handle_player_status_change(cJSON* root){
/*
json e.g.

{
	"jsonrpc":	"2.0",
	"method":	"player_status_change",
	"params":	{
		"volume":	37,
		"status":	"play",
		"source":	"cloud",
		"audioId":	"1796804142",
		"audioUid":	"",
		"audioSource":	"xiami",
		"audioName":	"冷暖",
		"audioAnchor":	"王俊凯",
		"audioAlbum":	"冷暖",
		"progress":	14,
		"audioExt":	"{\"playlist_request_id\":\"0be5426315112520638558802d0656\",\"audioType\":\"music\",\"audioLength\":\"234\",\"request_id\":\"0be5426315112523136211098d0656\"}"
	}
}
*/
    FUNCTION_BEGIN
    
    cJSON * params = NULL;
    cJSON * audioName = NULL;
    char* strName = NULL;
    cJSON * audioAuthor = NULL;
    char* strAuthor = NULL;
    cJSON * playerStatus = NULL;
    char* strStatus = NULL;
    PLAYER_STATUS_E status=GV_PLAYER_UNKNOWN;
    params = cJSON_GetObjectItem(root, G_ROOT_KEY_PARAMS);
    audioName = cJSON_GetObjectItem(params, G_PARAMS_KEY_AUDIO_NAME);
    if(NULL != audioName){
        strName = audioName->valuestring;
    }
    audioAuthor = cJSON_GetObjectItem(params, G_PARAMS_KEY_AUDIO_AUTHOR);
    if(NULL != audioAuthor){
        strAuthor = audioAuthor->valuestring;
    }
    playerStatus = cJSON_GetObjectItem(params, G_PARAMS_KEY_PLAYER_STATUS);
    if(NULL != playerStatus){
        strStatus = playerStatus->valuestring;
    }
    LOGD("get PlayerStatusMsg: name=[%s] author=[%s] status=[%s]",
            strName,
            strAuthor,
            strStatus);
    if(!strcmp(strStatus, G_PARAMS_VALUE_PLAYER_STATUS_PLAY)){
        if(LCTX.player_status_ctx.player_playmode==GV_PLAYER_PLAY_MEDIAKEY){
            LCTX.player_status_ctx.player_playmode=GV_PLAYER_PLAY_AUTO; 
        }else if(LCTX.player_status_ctx.player_playmode==GV_PLAYER_PLAY_HOLD){
            LCTX.player_status_ctx.player_playmode=GV_PLAYER_PLAY_AUTO;
        }
            
        status = GV_PLAYER_PLAY;
            
    }else if(!strcmp(strStatus, G_PARAMS_VALUE_PLAYER_STATUS_STOP)){
        if(LCTX.player_status_ctx.player_playmode==GV_PLAYER_PLAY_MEDIAKEY){
            LCTX.player_status_ctx.player_playmode=GV_PLAYER_PLAY_AUTO;
            status=GV_PLAYER_UNKNOWN;         
        }else if(LCTX.player_status_ctx.player_playmode==GV_PLAYER_PLAY_AUTO){ 
            status=GV_PLAYER_STOP;
        }

        

    } else if (!strcmp(strStatus, G_PARAMS_VALUE_PLAYER_STATUS_PAUSE)){
        status = GV_PLAYER_PAUSE;
    } else {
        LOGE("unknown player status: %s", strStatus);
        status = GV_PLAYER_UNKNOWN;
    }
    logic_handle_player_status(&LCTX, strName, strAuthor, status);
    FUNCTION_END
    return GV_OK;
}

static int _json_handle_network_status_change(cJSON* root){
    FUNCTION_BEGIN
    char * strStatus;
    cJSON * params = NULL;
    cJSON * networkStatus = NULL;
    NETWORK_STATUS_E status;
    
    params = cJSON_GetObjectItem(root, G_ROOT_KEY_PARAMS);
    networkStatus = cJSON_GetObjectItem(params, G_PARAMS_KEY_NETWORK_STATUS);
    if(NULL == networkStatus){
        LOGE("Network status json has no status param property, ignore it!");
        return GV_FAIL;
    }
    strStatus = networkStatus->valuestring;
    LOGD("get NetworkStatusMsg, status=[%s]", strStatus);
    if(!strcmp(strStatus, G_PARAMS_VALUE_NETWORK_STATUS_CONNECT)){
        status = GV_NETWORK_CONNECTED;
    } else if(!strcmp(strStatus, G_PARAMS_VALUE_NETWORK_STATUS_DISCONNECT)){
        status = GV_NETWORK_DISCNNECTED;
    } else {
        LOGE("Unknown network status: %s", strStatus);
        status = GV_NETWORK_UNKNOWN;
    }
    logic_handle_network_status(&LCTX, status);
    FUNCTION_END
    return GV_OK;
}

static int _json_handle_system_status_change(cJSON *root){
    FUNCTION_BEGIN
    cJSON * params = NULL;
    cJSON * jStatus = NULL;
    char * sStatus = NULL;
    params = cJSON_GetObjectItem(root, G_ROOT_KEY_PARAMS);
    jStatus = cJSON_GetObjectItem(params, G_PARAMS_KEY_SYSTEM_STATUS);
    sStatus = jStatus->valuestring;
    if(!strcmp(sStatus, G_PARAMS_VALUE_SYSTEM_STATUS_STANDBY)){
        logic_handle_system_status(&LCTX, GV_SYSTEM_STANDBY);
    } else {
        logic_handle_system_status(&LCTX, GV_SYSTEM_UNKNOWN);
    }
    
    FUNCTION_END
	return GV_OK;
}

static int _json_handle_lamp_control(cJSON * root){
    cJSON * params;
    cJSON * isOnObj;
    int isOn;

    params = cJSON_GetObjectItem(root, G_ROOT_KEY_PARAMS);
    if(NULL == params){
        LOGE("no params obj in hibernation control command!");
        return GV_FAIL;
    }
    isOnObj = cJSON_GetObjectItem(params, G_PARAMS_KEY_LED_STATUS);
    if(NULL == isOnObj){
        LOGE("no delaySeconds obj in params of hibernation control command!");
        return GV_FAIL;
    }
    isOn = !strcmp(G_PARAMS_VALUE_LED_STATUS_ON, isOnObj->valuestring);
    return logic_handle_led_control(isOn);

}

static int _json_handle_childlock_control(cJSON * root){
    cJSON * params;
    cJSON * isOnObj;
    int isOn;
    params = cJSON_GetObjectItem(root, G_ROOT_KEY_PARAMS);
    if(NULL == params){
        LOGE("no params obj in hibernation control command!");
        return GV_FAIL;
    }
    isOnObj = cJSON_GetObjectItem(params, G_PARAMS_KEY_CHILDLOCK_STATUS);
    if(NULL == isOnObj){
        LOGE("no delaySeconds obj in params of hibernation control command!");
        return GV_FAIL;
    }
    isOn = !strcmp(G_PARAMS_VALUE_CHILDLOCK_STATUS_ON, isOnObj->valuestring);
    return logic_handle_childlock_control(isOn);
}

static int _json_handle_configuration_reset(){
    return logic_handle_config_reset();
}

static int _json_handle_set_hibernation_control(cJSON * root){
    cJSON * params;
    cJSON * delaySecObj;
    int delaySec;
    params = cJSON_GetObjectItem(root, G_ROOT_KEY_PARAMS);
    if(NULL == params){
        LOGE("no params obj in hibernation control command!");
        return GV_FAIL;
    }
    delaySecObj = cJSON_GetObjectItem(params, G_PARAMS_KEY_HIBERNATION_DELAY);
    if(NULL == delaySecObj){
        LOGE("no delaySeconds obj in params of hibernation control command!");
        return GV_FAIL;
    }
    delaySec = delaySecObj->valueint;
    logic_handle_hibernationTime_save(delaySec);
    return logic_handle_hibernation_control(delaySec);
}

static int _json_handle_control_genie_speak(){
    logic_handle_genie_speak_led_control();
    return GV_OK;
}

static int _json_handle_play_tts_done(){
    logic_handle_play_tts_done();
    return GV_OK;
}

static int _json_handle_play_prompt_done(){
    logic_handle_play_prompt_done();     
    return GV_OK;
}
static int _json_handle_wifi_setup_result(cJSON * root){
    logic_handle_wifi_setup_result();
    return GV_OK;
}

int handle_engine_json(cJSON* root){
    FUNCTION_BEGIN
  	char cmd_method[G_JSON_MAX_LENGTH_METHOD] = {0};
    int ret = GV_FAIL;
    if (NULL == root)
    {
        LOGE("parse json string failed!");
        goto exit;
    }
    
    cJSON* method = cJSON_GetObjectItem(root, G_ROOT_KEY_METHOD);
    if(NULL == method){
        LOGE("Received engine JSON method is null");
        goto exit;
    }
    strncpy(cmd_method,
            method->valuestring,
            G_JSON_MAX_LENGTH_METHOD);
    LOGD("cmd_method:%s", cmd_method);
    if(!strcmp(cmd_method, G_ROOT_VALUE_METHOD_PLAYER_STATUS)) {
        LOGD("Received a player status change notify.");
        ret = _json_handle_player_status_change(root);
    } else if(!strcmp(cmd_method, G_ROOT_VALUE_METHOD_NETWORK_STATUS)) {
        LOGD("Received a network status change notify.");
        ret = _json_handle_network_status_change(root);
    } else if(!strcmp(cmd_method, G_ROOT_VALUE_METHOD_HANDSHAKE)) {
        LOGD("Received handshake from engine");
        LCTX.is_handshake_with_engine_done = TRUE;
    } else if(!strcmp(cmd_method, G_ROOT_VALUE_METHOD_SYSTEM_STATUS)) {
        LOGD("Received system status change notify.");
        ret = _json_handle_system_status_change(root);
    } else if(!strcmp(cmd_method, G_ROOT_VALUE_METHOD_CLOUD_READY)) {
        LOGD("Received cloud ready!");
        ret = logic_handle_cloud_ready();
    } else if(!strcmp(cmd_method, G_ROOT_VALUE_METHOD_CONTROL_LED)) {
        LOGD("Received led control");
        ret = _json_handle_lamp_control(root);
    } else if(!strcmp(cmd_method, G_ROOT_VALUE_METHOD_CONTROL_CHILDLOCK)) {
        LOGD("Received childlock control");
        ret = _json_handle_childlock_control(root);
    } else if(!strcmp(cmd_method, G_ROOT_VALUE_METHOD_CONTROL_RESET)) {
        LOGD("Received configuration reset control");
        ret = _json_handle_configuration_reset();
    } else if(!strcmp(cmd_method, G_ROOT_VALUE_METHOD_CONTROL_HIBERNATION)) {
        LOGD("Received hibernation control");
        ret = _json_handle_set_hibernation_control(root);
    }else if(!strcmp(cmd_method, G_ROOT_VALUE_METHOD_CONTROL_GENIE_SPEAK)){
        LOGD("Received control genie speak");
        ret = _json_handle_control_genie_speak();
    }else if(!strcmp(cmd_method, G_ROOT_VALUE_METHOD_PLAY_TTS_DONE)){
        LOGD("Received play tts done");   
        ret = _json_handle_play_tts_done();
    }else if(!strcmp(cmd_method,G_ROOT_VALUE_METHOD_PLAY_PROMPT_DONE)){
        ret = _json_handle_play_prompt_done();    
    }else if(!strcmp(cmd_method, G_ROOT_VALUE_METHOD_WIFI_SETUP_RESULT)){
        LOGD("Received play tts done");
        ret = _json_handle_wifi_setup_result(root);
    }else if(!strcmp(cmd_method, G_ROOT_VALUE_METHOD_ERASE_USER_DATA)){
		LOGD("Received erase user data control, factory reset.");
		logic_handle_factory_reset();
    }else if(!strcmp(cmd_method, G_ROOT_VALUE_METHOD_NEW_VOICE_MSG)){
        LOGD("Received a new voice message.");
        logic_handle_new_voice_message();
    }else if(!strcmp(cmd_method, G_ROOT_VALUE_METHOD_VOICE_MSG_PLAYED)){
        LOGD("Received voice message played notify.");
        logic_handle_voice_message_played();
    }else{
        LOGE("Unknown method:%s", cmd_method);
        goto exit;
    }

    exit:
    
    FUNCTION_END
    return ret;

}

