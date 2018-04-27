/*****************************************************************
*
* 启程-ALi合作代码
*
* 仅供与ALi人工智能实验室内部开发使用
* 禁止任何形式的开源或传播
*
* Author: liuwei01@qichengtec.com
*
*****************************************************************/

#ifndef _MEDIA_MANAGER_H_
#define _MEDIA_MANAGER_H_

#ifdef __cplusplus
extern "C" {
#endif

#undef EXTERN
#ifdef _MEDIA_MANAGER_IMPL_
#define EXTERN
#else
#define EXTERN extern
#endif
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <linux/input.h>
#include <math.h>
#include <sys/shm.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>

#include <pthread.h>
#include "genievendor.h"
#include "linux_list.h"

#undef LOG_TAG
#define LOG_TAG "MediaManager"

#define MAXLEN 						1024
#define PLAY_DIR_NUM	            4
#define MEM_ROOT_PATH               "/mnt/sdcard/"
#define FAVORITE_STORE_FOLDER       "/mnt/sdcard" 

/*本地播放tone*/
#define TONE_FILE_MUSIC             "/home/prompt/music.mp3"
#define TONE_FILE_SINOLOGY          "/home/prompt/sinology.mp3"
#define TONE_FILE_STORY             "/home/prompt/story.mp3"

/*电量tone*/
#define TONE_BATTERY_NEED_CHARGE            "/home/prompt/low_power_2.mp3"
#define TONE_BATTTERY_ENERGEY_NOT_ENOUGH    "/home/prompt/low_power_1.mp3"
#define TONE_BATTERY_CHARING                "/home/prompt/charging.mp3"
/*并网tone*/
#define TONE_PLEASE_CONNECT_NETWORK "/home/prompt/please_connect_to_the_network.mp3"

/*收藏tone*/
#define TONE_ADD_FAV_SUC "/home/prompt/add_fav_suc.mp3"
#define TONE_ADD_FAV_FAIL "/home/prompt/add_fav_fail.mp3"
#define TONE_CANCEL_FAV_SUC  "/home/prompt/cancel_fav_suc.mp3"
#define TONE_CANCEL_FAV_FAIL "/home/prompt/cancel_fav_fail.mp3"
#define TONE_NO_FAV "/home/prompt/no_fav.mp3"

#define BADNAME 			((int)-5) 	//无效文件名
#define FUHAO 				((int)-4)  	//首字符为符号
#define HANZI 				((int)-3) 	//首字符汉字
#define DAZIMU 				((int)-2)   //首字符大字母
#define XIAOZIMU			((int)-1)   //首字符小字母

//双向循环链表节点
typedef struct filenode{
	bool unread; 
	char *fullname;
	struct list_head list;
}T_Node, *PT_Node;

typedef enum {
    M_TYPE_BATTERY_LOW_20,
    M_TYPE_BATTERY_LOW_10,
    M_TYPE_BATTERY_LOW_POWER_OFF,
    M_TYPE_SINOLOGY,
    M_TYPE_STORY,
    M_TYPE_SONG,
    M_TYPE_FAVOURITE,
} MEDIA_TYPE_E;

typedef enum {
    ADD_SUCCESS = 0,    
    ADD_FAIL,
    DEL_SUCCESS,
    DEL_FAIL,
    ADD_EMPTY,
} FAV_RETVALUE_E;

extern pthread_mutex_t sdscanlock;
extern pthread_cond_t sdscancond;
//初始化函数开线程扫描T卡、分类保存索引，不要block调用
extern int media_manager_init(pthread_t* thread);

//返回初始化是否完成
//extern BOOL is_media_manager_inited();

//根绝给定的媒体类型，返回一个媒体文件路径
//成功则path为路径，返回值GV_OK
//失败则path为NULL，返回值GV_FAIL
extern int get_media_path_by_type(char** path, MEDIA_TYPE_E type); 

//这两个函数的出参，返回与当前播放类型相同的上一个/下一个媒体文件路径
//如果get_media_path_by_type从未调用过，则函数返回值为GV_FAIL，path为NULL
//否则path为路径，返回值GV_OK
extern int get_media_path_next(char** path);
extern int get_media_path_previous(char** path);
extern int get_sdcard_scanstatus(void);
void show(struct list_head *_headnode);
extern int playListInit(void);
extern void playListDelete(void);

extern int search_media(const char *media, struct list_head **_headnode);
extern int search_favourite_media(struct list_head **_headnode);
extern void clean_list(struct list_head* head);
extern int operate_is_favouriteFile_empty(void);
extern int operate_mediapath_favourite(void);
extern int add_mediapath_favourite(void);
extern int del_mediapath_favourite(void);
extern int get_player_media_type(void);
#ifdef __cplusplus
}
#endif

#endif /*_MEDIA_MANAGER_H_*/
