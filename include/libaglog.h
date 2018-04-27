/*********************************************************/
/*********************************************************/
/*                 AliGenie Log Module                   */
/*                  USER INTRODUCTION                    */
/*                                                       */
/*     (Encode: UTF-8, Language: Simplified Chinese)     */
/*********************************************************/
/*********************************************************/
/*
///////////////////////////////////////////////////////////
//AliGenieSDK日志模块使用说明

#include "libaglog.h"

//定义LOG_TAG,不定义默认是__FILE__
#undef LOG_TAG
#define LOG_TAG                 "MODULETAG"

//定义LOG_LEVEL,不定义默认是LOG_LEVEL_WARN
#undef  LOG_LEVEL
#define LOG_LEVEL                LOG_LEVEL_DEBUG 

//【可选】定义日期时间的背景色, 默认为黑色
#undef LOG_COLOR_TIME_BG
#define LOG_COLOR_TIME_BG       LOG_COLOR_BG_BLACK

//【可选】定义日期时间的文字色, 默认为白色
#undef LOG_COLOR_TIME_FONT
#define LOG_COLOR_TIME_FONT     LOG_COLOR_FONT_WHITE

//【可选】定义TAG的背景色, 默认为黑色
#undef LOG_COLOR_TAG_BG
#define LOG_COLOR_TAG_BG        LOG_COLOR_BG_BLACK

//【可选】定义TAG的文字色, 默认为白色
#undef LOG_COLOR_TAG_FONT
#define LOG_COLOR_TAG_FONT      LOG_COLOR_FONT_WHITE

//【可选】定义内容的背景色, 默认为黑色
#undef LOG_COLOR_CONTENT_BG
#define LOG_COLOR_CONTENT_BG    LOG_COLOR_BG_BLACK

//【可选】定义内容的文字色, 默认为白色
#undef LOG_COLOR_CONTENT_FONT
#define LOG_COLOR_CONTENT_FONT  LOG_COLOR_FONT_WHITE

///////////////////////////////////////////////////////////
//LOG调用

//输出error级LOG:
LOGE(format, args);

//输出warning级LOG:
LOGW(format, args);

//输出debug级LOG:
LOGD(format, args);

//输出verbose级LOG:
LOGV(format, args);

//函数进出日志, 使用时【末尾不要加分号】,LOGV级别开启
int foo()
{
    FUNCTION_BEGIN
	//do something
    FUNCTION_END
    return 0;
}

///////////////////////////////////////////////////////////
//注意事项
//  1. 背景色(LOG_COLOR_BG_XXX)和文字色(LOG_COLOR_FONT_XXX)
//     即使颜色相同, 值也是区别的, 不可混用;
//  2. 如果各颜色宏不定义, 默认背景为黑色, 文字为白色;
//  3. 代码中可以直接调用_log_print函数打印log, 若如此做, 
//     前6个参数需要自行传入各所需的颜色(详见函数声明),
//     此时颜色宏定义是无效的;
//  4. LOGX宏内format和args格式同printf函数, 会自动补上换行符, 
//     format格式字符串的末尾不需要显式加入\n换行;
//  5. 单条LOG最长可以显示1024字节(包含时间和TAG),
//     超出部分    会被截断不予显示.
//  6. FUNCTION_BEGIN、FUNCTION_END会使用LOGV级别输出,
///////////////////////////////////////////////////////////
**********************************************************/


#ifndef _LIBAGLOG_H_
#define _LIBAGLOG_H_

#ifdef __cplusplus  
extern "C" {  
#endif  

/*log级别定义*/
#define LOG_LEVEL_NONE              0
#define LOG_LEVEL_ERROR             1
#define LOG_LEVEL_WARN              2
#define LOG_LEVEL_DEBUG             3
#define LOG_LEVEL_VERBOSE           4
#define LOG_LEVEL_ALL               9

/*可选背景颜色, optional background colors*/
#define LOG_COLOR_BG_BLACK          40
#define LOG_COLOR_BG_RED            41
#define LOG_COLOR_BG_GREEN          42
#define LOG_COLOR_BG_YELLOW         43
#define LOG_COLOR_BG_BLUE           44
#define LOG_COLOR_BG_PURPLE         45
#define LOG_COLOR_BG_DART_GREEN     46
#define LOG_COLOR_BG_WHITE          47

/*可选字体颜色, optional font colors*/
#define LOG_COLOR_FONT_BLACK        30
#define LOG_COLOR_FONT_RED          31
#define LOG_COLOR_FONT_GREEN        32
#define LOG_COLOR_FONT_YELLOW       33
#define LOG_COLOR_FONT_BLUE         34
#define LOG_COLOR_FONT_PURPLE       35
#define LOG_COLOR_FONT_DARK_GREEN   36
#define LOG_COLOR_FONT_WHITE        37

/*LOG TAG*/
#define LOG_TAG __FILE__

/*颜色定义*/
#define LOG_COLOR_TIME_BG           LOG_COLOR_BG_BLACK
#define LOG_COLOR_TIME_FONT         LOG_COLOR_FONT_WHITE
#define LOG_COLOR_TAG_BG            LOG_COLOR_BG_GREEN
#define LOG_COLOR_TAG_FONT          LOG_COLOR_FONT_WHITE
#define LOG_COLOR_CONTENT_BG        LOG_COLOR_BG_BLACK
#define LOG_COLOR_CONTENT_FONT      LOG_COLOR_FONT_GREEN


#define LOG(...)    _log_print(LOG_COLOR_TIME_BG,LOG_COLOR_TIME_FONT,LOG_COLOR_TAG_BG,LOG_COLOR_TAG_FONT,LOG_COLOR_CONTENT_BG,LOG_COLOR_CONTENT_FONT,__VA_ARGS__)

#define LOGE(...)   LOG(LOG_TAG, LOG_LEVEL_ERROR, __VA_ARGS__)
#define LOGW(...)   LOG(LOG_TAG, LOG_LEVEL_WARN, __VA_ARGS__)
#define LOGD(...)   LOG(LOG_TAG, LOG_LEVEL_DEBUG, __VA_ARGS__)
#define LOGV(...)   LOG(LOG_TAG, LOG_LEVEL_VERBOSE, __VA_ARGS__)

#define FUNCTION_BEGIN 	LOGV("function %s line %d begin", __FUNCTION__, __LINE__);
#define FUNCTION_END 	LOGV("function %s line %d end", __FUNCTION__, __LINE__);

extern void _log_print(
                        int time_color_bg,
                        int time_color_font,
                        int tag_color_bg,
                        int tag_color_font,
                        int content_color_bg,
                        int content_color_font,
                        const char* tag,
                        int loglevel,
                        const char* format,
                        ...);

extern int getAGLogVersion();
extern void aglog_enable(int isEnable);
extern void aglog_enable_switch();
extern void aglog_change_level();
extern void aglog_set_level(int level);
extern void aglog_read_config(const char * config_file_path);

#ifdef __cplusplus  
}
#endif  

#endif /*_LIBAGLOG_H_*/
