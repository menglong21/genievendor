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

#ifndef _GENIEVENDOR_H_
#define _GENIEVENDOR_H_

#include "backtrace.h"
#include "aligeniesdk_json_api.h"
#include "libaglog.h"

/*函数返回值*/
#define GV_OK       ((int)0)
#define GV_FAIL     ((int)-1)
#define GV_ERR      ((int)2)

/*定义布尔型*/
#undef BOOL
typedef unsigned char BOOL;

/*定义常量*/
#ifndef NULL
#define NULL        (0)
#endif /*NULL*/
#ifndef TRUE
#define TRUE        ((unsigned char)1)
#endif /*TRUE*/
#ifndef FALSE
#define FALSE       ((unsigned char)0)
#endif /*FALSE*/

#define ERROR  1
#define OK     0

/*设备配置*/
//音量调整步长
#define VOLUME_STEP_LENGTH      (20)

#endif /*_GENIEVENDOR_H_*/
