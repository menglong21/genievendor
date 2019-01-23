/*****************************************************************
*
* 
*****************************************************************/

#ifndef _JSON_PARSER_H_
#define _JSON_PARSER_H_

#ifdef __cplusplus  
extern "C" {  
#endif  

#undef EXTERN
#ifdef _JSON_PARSER_IMPL_
#define EXTERN
#else
#define EXTERN extern
#endif

#include <unistd.h>
#include "genievendor.h"
#include "cJSON.h"
#include "keymanager.h"

EXTERN int handle_engine_json(cJSON* root);
EXTERN int json_pack_handshake(char* buf);
EXTERN int json_pack_childlock_status(char *buf, const char* status);
EXTERN int json_pack_keyevent(char * jsonstr, const char * keyname);
EXTERN int json_pack_led_status(char* buf, const char * status);
EXTERN int json_pack_battery_status(char* buf, const int percent);
EXTERN int json_pack_media_play(char * buf, const char * path);
EXTERN int json_pack_prompt_play(char *buf,const char *path);
EXTERN int json_pack_player_pause(char * buf);
EXTERN int json_pack_player_resume(char * buf);
EXTERN int json_pack_player_stop(char * buf);
EXTERN int json_pack_volume_adjust(char *buf, const char *buttonName, int step_length);

#ifdef __cplusplus  
}
#endif  

#endif /*_JSON_PARSER_H_*/
