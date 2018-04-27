/*****************************************************************
*
* 阿里巴巴人工智能实验室 厂商开放代码
*
* 仅供与人工智能实验室的合作厂商内部开发使用
* 禁止任何形式的开源或传播
* Author:liuwei01@qichengtec.com
* Funciton:local audio play sd card scan and meidia play button
           interface v1.0 
  Date:2018.1.8
* Author: humenglong@qichengtec.com
* Funciton:Add local meida file sort 
* Date:
* Author: zhuqingshui@qichengtec.com
* Function:Add local favourtie file function 
* Date:
*****************************************************************/

/*****************************************************************
* 模块名称
*
* GenieVendor多媒体播放
*
* 模块功能
*
* 本地媒体管理
*
*****************************************************************/

#define _MEDIA_MANAGER_IMPL_

#include <memory.h>
#include "mediamanager.h"
#include "logic.h"

#include "libaglog.h"
#undef LOG_TAG
#define LOG_TAG "GVMediaManager"

struct list_head *gPlayListHead[PLAY_DIR_NUM];
struct list_head *music_headnode = NULL;
struct list_head *story_headnode = NULL;
struct list_head *sinology_headnode = NULL;
struct list_head *favourite_headnode = NULL;
struct list_head *headnode;
struct list_head *playlist_head;

const char *playDir[PLAY_DIR_NUM] = {
	[0] = "音乐",
	[1] = "故事",
	[2] = "国学",
    [3] = FAVORITE_STORE_FOLDER, 
};

//Ali player support music format
const char *support_type[2] = {
	[0] = ".mp3",
	[1] = ".m4a"
};
	
const char rootPath[] = {MEM_ROOT_PATH};
const char FILE_LIST_PATH[] = {MEM_ROOT_PATH"list.txt"};
const char FILE_FAVOURITE_PATH[] = {MEM_ROOT_PATH"favourite.dat"};
int playDirNum = -1;
pthread_mutex_t sdscanlock;
pthread_cond_t sdscancond;


void * _scan_sdcard_thread(void * arg)
{
	int ret = -2;
    SDCARD_STATUS_E status;

    while(1){
	
        pthread_mutex_lock(&sdscanlock);
        pthread_cond_wait(&sdscancond,&sdscanlock);
        if (LCTX.player_sd_status_ctx.SDcardRefreshFlag)
		{
            LCTX.player_sd_status_ctx.SDcardRefreshFlag = FALSE;
            status = GV_SDCARD_SCAN_ING;
            ret = playListInit();
	        if (ret == GV_OK)
			{
                status = GV_SDCARD_SCAN_OK;
                LOGD("SDCard scan success");
            }
			else if (ret == GV_FAIL)
			{     
                status = GV_SDCARD_SCAN_FAILED; 
                LOGD("SDCard scan failed");
            }    
            logic_handle_sd_status_change(&LCTX,status); 
        }
		else
		{      
            if(get_sdcard_scanstatus() == GV_SDCARD_UMOUNT)
			{
            	LOGD("SDCard is umounted");
            	playListDelete();
            }
        }

        pthread_mutex_unlock(&sdscanlock);
        usleep(1 * 1000 * 1000);
    }           
	return (void *)NULL;
}


//初始化函数开线程扫描T卡、分类保存索引，不要block调用
int media_manager_init(pthread_t* thread)
{	
    FUNCTION_BEGIN
	int ret;
    LCTX.player_sd_status_ctx.SDcardRefreshFlag=TRUE;    
    LCTX.player_sd_status_ctx.sd_status=GV_SDCARD_SCAN_NOTSTART;
    ret = pthread_create(thread, NULL, _scan_sdcard_thread, NULL);
    if(0 == ret)
	{
        LOGD("sdcard scan thread created!");
        FUNCTION_END
        return GV_OK;
    } 
	else 
	{
        LOGE("sdcard scan thread create failed!");
        FUNCTION_END
        return GV_FAIL;
    }
}


//根绝给定的媒体类型，返回媒体文件路径
//成功则path为路径，返回值GV_OK
//失败则path为NULL，返回值GV_FAIL
int get_media_path_by_type(char** path, MEDIA_TYPE_E type)
{
    FUNCTION_BEGIN
    //LOGD("g_path: %08x", g_path);
    //*path = g_path;
    int ret = GV_FAIL;
	PT_Node tmp;
	if(get_sdcard_scanstatus() == GV_SDCARD_SCAN_OK)
	{		
		switch(type) 	
		{
			case M_TYPE_SONG:
				if (!LCTX.is_music_empty)
				{
					playlist_head = gPlayListHead[0]->next;		
				}
			 	break;
			case M_TYPE_STORY:
				if (!LCTX.is_story_empty)
				{
			 		playlist_head = gPlayListHead[1]->next;
				}
			 	break;
			case M_TYPE_SINOLOGY:
				if (!LCTX.is_sinology_empty)
				{
			 		playlist_head = gPlayListHead[2]->next;
				}
			 	break;		
            case M_TYPE_FAVOURITE:
                               if(!LCTX.is_favourite_empty)
                               {
                        		playlist_head = gPlayListHead[3]->next;
                               }

				break;
			default:
				break;
		}
        
        if(playlist_head != NULL)
		{          
			printf("playlist_head not NULL\n");
            if ((type == M_TYPE_SONG) && !LCTX.is_music_empty)
			{
				music_headnode = playlist_head;
				tmp = list_entry(music_headnode, T_Node, list);
				if (tmp->fullname)
				{
					*path = tmp->fullname;
				}
				else
				{
					*path = NULL;
				}
				story_headnode = NULL;
				sinology_headnode = NULL;    
				favourite_headnode = NULL;
				LCTX.player_status_ctx.player_mediatype = M_TYPE_SONG;
            }
			else if((type == M_TYPE_STORY) && !LCTX.is_story_empty)
			{
				story_headnode = playlist_head;
				tmp = list_entry(story_headnode, T_Node, list);
				if (tmp->fullname)
				{
					*path = tmp->fullname;
				}
				else
				{
					*path = NULL;
				}
				music_headnode = NULL;
				sinology_headnode = NULL;
				favourite_headnode = NULL;
				LCTX.player_status_ctx.player_mediatype = M_TYPE_STORY;
            }
			else if((type == M_TYPE_SINOLOGY) && !LCTX.is_sinology_empty)
			{
				sinology_headnode = playlist_head;
				tmp = list_entry(sinology_headnode, T_Node, list);
				if (tmp->fullname)
				{
					*path = tmp->fullname;
				}
				else
				{
					*path = NULL;
				}
				music_headnode = NULL;
				story_headnode = NULL;   
				favourite_headnode = NULL;
				LCTX.player_status_ctx.player_mediatype = M_TYPE_SINOLOGY;
            }
			else if((type == M_TYPE_FAVOURITE) &&(!LCTX.is_favourite_empty))
			{
				favourite_headnode = playlist_head;
				tmp = list_entry(favourite_headnode, T_Node, list);
				if (tmp->fullname)
				{
					*path = tmp->fullname;
				}
				else
				{
					*path = NULL;
				}
				music_headnode = NULL;
				sinology_headnode = NULL;
				story_headnode = NULL;
				LCTX.player_status_ctx.player_mediatype = M_TYPE_FAVOURITE;
            }      
            
            if(NULL == (*path))
			{
			    ret = GV_FAIL; 	
		    }
			else
			{
			    ret = GV_OK;
			    LOGD("*path: %08x", *path);
		    } 
        }
		else
		{
           ret = GV_FAIL;           
           printf("NO meida file,please copy\n");
        }
	}
	else if (get_sdcard_scanstatus() == GV_SDCARD_SCAN_ING)
	{
        *path=NULL;
		ret = GV_FAIL;
		LOGD("SDCard is scanning folder...");
	}
	else if (get_sdcard_scanstatus() == GV_SDCARD_SCAN_FAILED)
	{
        *path=NULL;
		ret = GV_FAIL;
		LOGD("SDCard scanning failed,card may be damaged");		
	}else if(get_sdcard_scanstatus() == GV_SDCARD_UMOUNT){
        *path=NULL;
        ret = GV_FAIL; 
        LOGD("SDCard is umounted");    
	}
    FUNCTION_END
	return ret;
}

//这两个函数的出参，返回与当前播放类型相同的上一个/下一个媒体文件路径
//如果get_media_path_by_type从未调用过，则函数返回值为GV_FAIL，path为NULL
//否则path为路径，返回值GV_OK
int get_media_path_next(char** path)
{
    FUNCTION_BEGIN
    //*path = g_path;
	int ret = GV_FAIL;
	PT_Node tmp = NULL;
	if(get_sdcard_scanstatus() == GV_SDCARD_SCAN_OK)
	{
		if ((NULL == music_headnode)&&(NULL == story_headnode)&&(NULL == sinology_headnode)&&(NULL == favourite_headnode))
		{
            LOGE("Now is no media content is playing");
			*path = NULL;
			ret = GV_FAIL;
		}
		else
		{
			if(music_headnode != NULL)
			{	
				//就剩一个节点
				if ((music_headnode->next->next == music_headnode) && (music_headnode->next == gPlayListHead[0]))
				{
					//printf("only one node\n");
					tmp = list_entry(music_headnode, T_Node, list);
					if (tmp->fullname)
					{
						*path = tmp->fullname;
					}
				}
				else if (music_headnode->next == gPlayListHead[0])//不止一个节点，下一个节点是头结点
				{
					//printf("not only one node, but next is head\n");
					music_headnode = music_headnode->next->next;
					tmp = list_entry(music_headnode, T_Node, list);
					if (tmp->fullname)
					{
						*path = tmp->fullname;
					}
				}
				else//不止一个节点，下一个节点不是头结点
				{
					//printf("not only one node, and next is not head\n");
					music_headnode = music_headnode->next;
					tmp = list_entry(music_headnode, T_Node, list);
					if (tmp->fullname)
					{
						*path = tmp->fullname;
					}
				}
			}
			else if(story_headnode != NULL)
			{
				//就剩一个节点
				if ((story_headnode->next->next == story_headnode) && (story_headnode->next == gPlayListHead[1]))
				{
					//printf("only one node\n");
					tmp = list_entry(story_headnode, T_Node, list);
					if (tmp->fullname)
					{
						*path = tmp->fullname;
					}
				}
				else if (story_headnode->next == gPlayListHead[1])//不止一个节点，下一个节点是头结点
				{
					//printf("not only one node, but next is head\n");
					story_headnode = story_headnode->next->next;
					tmp = list_entry(story_headnode, T_Node, list);
					if (tmp->fullname)
					{
						*path = tmp->fullname;
					}
				}
				else//不止一个节点，下一个节点不是头结点
				{
					//printf("not only one node, and next is not head\n");
					story_headnode = story_headnode->next;
					tmp = list_entry(story_headnode, T_Node, list);
					if (tmp->fullname)
					{
						*path = tmp->fullname;
					}
				}
			}
			else if(sinology_headnode != NULL)
			{
				//就剩一个节点
				if ((sinology_headnode->next->next == sinology_headnode) && (sinology_headnode->next == gPlayListHead[2]))
				{
					//printf("only one node\n");
					tmp = list_entry(sinology_headnode, T_Node, list);
					if (tmp->fullname)
					{
						*path = tmp->fullname;
					}
				}
				else if (sinology_headnode->next == gPlayListHead[2])//不止一个节点，下一个节点是头结点
				{
					//printf("not only one node, but next is head\n");
					sinology_headnode = sinology_headnode->next->next;
					tmp = list_entry(sinology_headnode, T_Node, list);
					if (tmp->fullname)
					{
						*path = tmp->fullname;
					}
				}
				else//不止一个节点，下一个节点不是头结点
				{
					//printf("not only one node, and next is not head\n");
					sinology_headnode = sinology_headnode->next;
					tmp = list_entry(sinology_headnode, T_Node, list);
					if (tmp->fullname)
					{
						*path = tmp->fullname;
					}
				}
			}
			else if(favourite_headnode != NULL)
			{
				//printf("get_media_path_next\n");
				//就剩一个节点
				if ((favourite_headnode->next->next == favourite_headnode) && (favourite_headnode->next == gPlayListHead[3]))
				{
					//printf("only one node\n");
					tmp = list_entry(favourite_headnode, T_Node, list);
					if (tmp->fullname)
					{
						*path = tmp->fullname;
					}
				}
				else if (favourite_headnode->next == gPlayListHead[3])//不止一个节点，下一个节点是头结点
				{
					//printf("not only one node, but next is head\n");
					favourite_headnode = favourite_headnode->next->next;
					tmp = list_entry(favourite_headnode, T_Node, list);
					if (tmp->fullname)
					{
						*path = tmp->fullname;
					}
				}
				else//不止一个节点，下一个节点不是头结点
				{
					//printf("not only one node, and next is not head\n");
					favourite_headnode = favourite_headnode->next;
					tmp = list_entry(favourite_headnode, T_Node, list);
					if (tmp->fullname)
					{
						*path = tmp->fullname;
					}
				}
            }    
			ret = GV_OK;
		}
	}
	else if(get_sdcard_scanstatus() == GV_SDCARD_SCAN_ING)
	{
		ret = GV_FAIL;        
		LOGD("SDCard is scanning folder...");
	}
	else if(get_sdcard_scanstatus() == GV_SDCARD_SCAN_FAILED)
	{
		ret = GV_FAIL;        
		LOGD("SDCard scanning failed,card may be damaged");
	}else if(get_sdcard_scanstatus() == GV_SDCARD_UMOUNT){
        *path=NULL;
		ret = GV_FAIL;
        LOGD("SDCard is umounted");
	}
    FUNCTION_END
	return ret;
}


int get_media_path_previous(char** path)
{
    FUNCTION_BEGIN
    //*path = g_path;
	int ret = GV_FAIL;
	PT_Node tmp = NULL;
	if(get_sdcard_scanstatus() == GV_SDCARD_SCAN_OK)
	{
		if((NULL == music_headnode)&&(NULL == story_headnode)&&(NULL == sinology_headnode)&&(NULL == favourite_headnode))
		{
            LOGE("Now is no media content is playing");          
			*path = NULL;
			ret = GV_FAIL;
		}
		else
		{
			if(music_headnode != NULL)
			{	
				//就剩一个节点
				if ((music_headnode->prev->prev == music_headnode) && (music_headnode->prev == gPlayListHead[0]))
				{
					//printf("only one node\n");
					tmp = list_entry(music_headnode, T_Node, list);
					if (tmp->fullname)
					{
						*path = tmp->fullname;
					}
				}
				else if (music_headnode->prev == gPlayListHead[0])//不止一个节点，上一个节点是头结点
				{
					//printf("not only one node, but prev is head\n");
					music_headnode = music_headnode->prev->prev;
					tmp = list_entry(music_headnode, T_Node, list);
					if (tmp->fullname)
					{
						*path = tmp->fullname;
					}
				}
				else//不止一个节点，上一个节点不是头结点
				{
					//printf("not only one node, and prev is not head\n");
					music_headnode = music_headnode->prev;
					tmp = list_entry(music_headnode, T_Node, list);
					if (tmp->fullname)
					{
						*path = tmp->fullname;
					}
				}
			}
			else if(story_headnode != NULL)
			{
				//就剩一个节点
				if ((story_headnode->prev->prev == story_headnode) && (story_headnode->prev == gPlayListHead[1]))
				{
					//printf("only one node\n");
					tmp = list_entry(story_headnode, T_Node, list);
					if (tmp->fullname)
					{
						*path = tmp->fullname;
					}
				}
				else if (story_headnode->prev == gPlayListHead[1])//不止一个节点，上一个节点是头结点
				{
					//printf("not only one node, but prev is head\n");
					story_headnode = story_headnode->prev->prev;
					tmp = list_entry(story_headnode, T_Node, list);
					if (tmp->fullname)
					{
						*path = tmp->fullname;
					}
				}
				else//不止一个节点，上一个节点不是头结点
				{
					//printf("not only one node, and prev is not head\n");
					story_headnode = story_headnode->prev;
					tmp = list_entry(story_headnode, T_Node, list);
					if (tmp->fullname)
					{
						*path = tmp->fullname;
					}
				}
			}
			else if(sinology_headnode != NULL)
			{
				//就剩一个节点
				if ((sinology_headnode->prev->prev == sinology_headnode) && (sinology_headnode->prev == gPlayListHead[2]))
				{
					//printf("only one node\n");
					tmp = list_entry(sinology_headnode, T_Node, list);
					if (tmp->fullname)
					{
						*path = tmp->fullname;
					}
				}
				else if (sinology_headnode->prev == gPlayListHead[2])//不止一个节点，上一个节点是头结点
				{
					//printf("not only one node, but prev is head\n");
					sinology_headnode = sinology_headnode->prev->prev;
					tmp = list_entry(sinology_headnode, T_Node, list);
					if (tmp->fullname)
					{
						*path = tmp->fullname;
					}
				}
				else//不止一个节点，上一个节点不是头结点
				{
					//printf("not only one node, and prev is not head\n");
					sinology_headnode = sinology_headnode->prev;
					tmp = list_entry(sinology_headnode, T_Node, list);
					if (tmp->fullname)
					{
						*path = tmp->fullname;
					}
				}
			}
			else if(favourite_headnode != NULL)
			{
				//就剩一个节点
				if ((favourite_headnode->prev->prev == favourite_headnode) && (favourite_headnode->prev == gPlayListHead[3]))
				{
					//printf("only one node\n");
					tmp = list_entry(favourite_headnode, T_Node, list);
					if (tmp->fullname)
					{
						*path = tmp->fullname;
					}
				}
				else if (favourite_headnode->prev == gPlayListHead[3])//不止一个节点，上一个节点是头结点
				{
					//printf("not only one node, but prev is head\n");
					favourite_headnode = favourite_headnode->prev->prev;
					tmp = list_entry(favourite_headnode, T_Node, list);
					if (tmp->fullname)
					{
						*path = tmp->fullname;
					}
				}
				else//不止一个节点，上一个节点不是头结点
				{
					//printf("not only one node, and prev is not head\n");
					favourite_headnode = favourite_headnode->prev;
					tmp = list_entry(favourite_headnode, T_Node, list);
					if (tmp->fullname)
					{
						*path = tmp->fullname;
					}
				}
            }   
			ret = GV_OK;
		}	
	}
	else if(get_sdcard_scanstatus() == GV_SDCARD_SCAN_ING)
	{
		ret = GV_FAIL;        
		LOGD("SDCard is scanning folder...");
	}
	else if(get_sdcard_scanstatus() == GV_SDCARD_SCAN_FAILED)
	{
		ret = GV_FAIL;        
		LOGD("SDCard scanning failed,card may be damaged");
	}else if(get_sdcard_scanstatus() == GV_SDCARD_UMOUNT){       
        *path=NULL;
		ret = GV_FAIL;
        LOGD("SDCard is umounted");
	}
    FUNCTION_END
	return ret;
}


int get_sdcard_scanstatus(void)
{
	return LCTX.player_sd_status_ctx.sd_status;
}


static void createDir(void)
{
    int i;
    char buf[128];

    for (i = 0; i < PLAY_DIR_NUM - 1; i++)
    {
        strcpy(buf, MEM_ROOT_PATH);
        strcat(buf, playDir[i]);
        if( 0 != access(buf, 0) )
        {
            strcpy(buf, "mkdir "MEM_ROOT_PATH);
            strcat(buf, playDir[i]);
            system(buf);
            //system("sync");
        }
    }
}


/* del playlst*/
void clean_list(struct list_head* head)
{
	struct list_head *pos;
	PT_Node tmp = NULL;
	list_for_each(pos, head)
	{
		tmp = list_entry(pos, T_Node, list);
		//list_del(pos);
		free(tmp->fullname);
		tmp->fullname = NULL;
		free(tmp);
		tmp = NULL;
	}
}


//遍历打印
void show(struct list_head *_headnode)
{
	struct list_head *pos;
	static int i = 1; 
	PT_Node tmp;
	list_for_each(pos, _headnode)
	{
		tmp = list_entry(pos, T_Node, list);
		LOGD("No.%05d -> %s\n", i++, tmp->fullname);
	}
}


int playListInit(void)
{
    FUNCTION_BEGIN
	//FILE *fp;
	char buf[36];
	int i, ret;
    createDir();
	
	if (playDirNum < 0)
	{
		for (i = 0; i < PLAY_DIR_NUM; i++)
		{
			gPlayListHead[i] = NULL;
		}        
        playDirNum = PLAY_DIR_NUM;
	}
	else
	{
		LOGD("playListDelete\n");
		playListDelete();
	}
    
	for (i = 0; i < PLAY_DIR_NUM; i++)
	{
        memset(buf, 0, sizeof(buf));
		gPlayListHead[i] = (struct list_head*)malloc(sizeof(struct list_head));
		if(gPlayListHead[i] == NULL)
		{
		   LOGE("gPlayListHead[%d] malloc failed\n",i);
		   return -1;
		}
		memset(gPlayListHead[i], 0, sizeof(struct list_head));
        if (i <= 2)
		{ 
			strcpy(buf, MEM_ROOT_PATH);
   			strcat(buf, playDir[i]);
         	        ret = search_media(buf, &gPlayListHead[i]);
                        if (0 != ret)
                        {
                           LOGE("ret is %d, search %s directory failed",ret, playDir[i]);
                            clean_list(gPlayListHead[i]);
                            gPlayListHead[i] = NULL;
                            LOGD("gPlayListHead[%d]=%0x", i, gPlayListHead[i]);
                            if (0 == i)
                            {
                                LCTX.is_music_empty = 1;
                            }
                            else if (1 == i)
                            {
                                LCTX.is_story_empty = 1;
                            }
                            else if (2 == i)
                            {
                                LCTX.is_sinology_empty = 1;
                            }
                       }
              }
              else if (i == 3)
                {
                      ret = search_favourite_media(&gPlayListHead[i]);
                      if(ret != 0)
                      {
                          LCTX.is_favourite_empty = 1;
                      }
               }
  
	}
		
	//playlist_head = gPlayListHead[playDirIndex];
	if ((NULL == gPlayListHead[0]) && (NULL == gPlayListHead[1]) && (NULL == gPlayListHead[2]) && (NULL == gPlayListHead[3]))
	{
        LOGE("no any media play list to create");
		return GV_FAIL;
	}
    
//	for (i = 0; i < PLAY_DIR_NUM; i++)
//	{
//		show(gPlayListHead[i]);
//	}
	return GV_OK;
}


void playListDelete(void)
{
	int i;

	for (i = 0; i < PLAY_DIR_NUM; i++)
	{
		if (gPlayListHead[i] != NULL)
		{
			clean_list(gPlayListHead[i]);
            gPlayListHead[i] = NULL;
		}
	}
    playDirNum = -1;

	playlist_head = NULL;
	music_headnode = playlist_head;
    sinology_headnode = playlist_head;
    story_headnode = playlist_head;
    favourite_headnode = playlist_head;
    LOGD("media play list have been clean");
}


static void init_list(struct list_head **p)
{
	*p = (struct list_head *)malloc(sizeof(struct list_head));
	if(*p == NULL)
	{
	     LOGE("struct list_head malloc failed\n");
		 return ;
	}
	INIT_LIST_HEAD(*p);
}


//校验文件后缀并获得文件序号,返回数字即数字，小写字母-1，大写字母-2，汉字-3，符号-4,文件名不对-5
static int get_num_from_medianame(const char *medianame)
{
	char buf1[MAXLEN];
	char buf2[MAXLEN];
	char _medianame[MAXLEN];
	char *tmp = strrchr(medianame, '/');
	if (tmp)
	{
		strncpy(_medianame, tmp+1, strlen(tmp+1)+1);//去掉路径获得文件名
	}
	strncpy(buf1, _medianame, strlen(_medianame)+1);
	char *p = strrchr(buf1, '.');
	if (!strncmp(p, support_type[0], 4) || !strncmp(p, support_type[1], 4))//校验后缀
	{
		int i = 0, j = 0;
		for (i = 0; i < strlen(_medianame); i++)
		{
			if (_medianame[i] >= '0' && _medianame[i] <= '9')
			{
				j++;
			}
			else
			{
				break;
			}
		}
		memset(buf2, 0, MAXLEN);
		if (j)
		{
			return atoi(strncpy(buf2, buf1, j));
		}
		else if (!j && ((_medianame[0]&0x80)) && (_medianame[1]&0x80))
		{
			return HANZI;
		}
		else if (!j && (((int)_medianame[0] >= 65) && ((int)_medianame[0] <= 90)))
		{
			return DAZIMU;
		}
		else if (!j && (((int)_medianame[0] >= 97) && ((int)_medianame[0] <= 120)))
		{
			return XIAOZIMU;
		}
		else
		{
			return FUHAO;
		}
	}
	else//后缀名不对
	{
		LOGD("medianame is not good!\n");
		return BADNAME;
	}
}


static int insert_by_order(const char *medianame, int length, struct list_head *head, int NoZimu)
{
    //FUNCTION_BEGIN
	int add_flag = 0;
	struct list_head *pos;
	PT_Node new, tmp;
	char newbuf[MAXLEN];
	char tmpbuf[MAXLEN];

	//分配新的节点
	new = (PT_Node)malloc(sizeof(T_Node));
	if(new == NULL)
	{
	    LOGE(" new node malloc failed\n");
		return -1;
	}
	new->unread = 0;
	new->fullname = (char *)malloc(length+1);
	strncpy(new->fullname, medianame, length+1);
	strncpy(newbuf, medianame, length+1);
	if (head->next == head)//如果链表为空
	{
		LOGD("list is empty\n");
		list_add(&new->list, head);
		return 0;
	}
	else if (NoZimu >= 0)//数字
	{
		list_for_each(pos, head)//数字正序遍历链表，寻找插入点
		{
			tmp = list_entry(pos, T_Node, list);
			memset(tmpbuf, 0, MAXLEN);
			if (NULL != tmp->fullname)
			{
				strncpy(tmpbuf, tmp->fullname, strlen(tmp->fullname)+1);
			}
			if (get_num_from_medianame(tmpbuf) > NoZimu)
			{
				list_add(&new->list, tmp->list.prev);
				add_flag = 1;//已经插入
				break;
			}
		}
	}
	else if (NoZimu == XIAOZIMU)//小写字母
	{
		list_for_each_prev(pos, head)//字母倒序遍历链表，寻找插入点
		{
			tmp = list_entry(pos, T_Node, list);
			memset(tmpbuf, 0, MAXLEN);
			if (NULL != tmp->fullname)
			{
				strncpy(tmpbuf, tmp->fullname, strlen(tmp->fullname)+1);
			}
			if (strcmp(tmpbuf, newbuf) < 0)
			{
				list_add(&new->list, pos);
				add_flag = 1;//已经插入
				break;					
			}
		}
	}
	else if (NoZimu == DAZIMU)//大写字母
	{
//		return 0;
	}
	else if (NoZimu == FUHAO)//符号
	{
		list_add(&new->list, head);
		add_flag = 1;//已经插入
	}
	else if (NoZimu == HANZI)//汉字
	{
		list_for_each_prev(pos, head)//字母倒序遍历链表，寻找插入点
		{
			tmp = list_entry(pos, T_Node, list);
			memset(tmpbuf, 0, MAXLEN);
			if (NULL != tmp->fullname)
			{
				strncpy(tmpbuf, tmp->fullname, strlen(tmp->fullname)+1);
			}
			if (strcmp(tmpbuf, newbuf) < 0)
			{
				list_add(&new->list, pos);
				add_flag = 1;//已经插入
				break;					
			}
		}
	}
	else if (NoZimu == BADNAME)
	{
		//do nothing
		return 0;
	}
	if (!add_flag)//没有插入
	{
		list_add_tail(&new->list, head);
	}
    //FUNCTION_END
	return 0;
}


//将media目录中的文件遍历排序，插入链表_headnode中
int search_media(const char *media, struct list_head **_headnode)
{
    FUNCTION_BEGIN
	DIR *dirp = NULL;
	struct dirent *entp = NULL;
	char buftmp[MAXLEN];
	int i = 0;

	init_list(_headnode);
	//打开目录
	if (NULL == (dirp = opendir(media)))
	{
		return -1;
	}

	//遍历目录
	while (NULL != (entp = readdir(dirp)))
	{
		if (entp->d_type & DT_DIR)//子目录不处理
		{
			LOGD("continue\n");
			continue;
		}
		else
		{
			i++;
			sprintf(buftmp, "%s/%s", media, entp->d_name);//文件全路径
			insert_by_order(buftmp, strlen(buftmp), *_headnode, get_num_from_medianame(buftmp));//按顺序插入节点
		}
	}
	//关闭目录
	closedir(dirp);
    FUNCTION_END
	if (i)
	{
		return 0;
	}
	else 
	{
		return -1;
	}
}


//打开收藏文件夹，建立收藏文件循环链表
int search_favourite_media(struct list_head **_headnode)
{
    FUNCTION_BEGIN
        FILE *fp = NULL;
	char buftmp[MAXLEN] = {0};

	init_list(_headnode);
// 将收藏文件列表路径插入到循环链表
       if((fp=fopen(FILE_FAVOURITE_PATH,"r")) != NULL && (fgetc(fp) != EOF)){
            fseek(fp,0L,SEEK_SET);
            while(!feof(fp)){
	        if(fgets(buftmp, MAXLEN, fp) != NULL)
	        {
		   int length = strlen(buftmp);
		   if (buftmp[length - 1] == '\n')
	           {
		      buftmp[length - 1] = '\0';
		   }
	          PT_Node new = (PT_Node)malloc(sizeof(T_Node));
		  new->unread = 0;
		  new->fullname = (char*)malloc(MAXLEN);
		  strncpy(new->fullname, buftmp, length + 1);
		  list_add_tail(&new->list, *_headnode);
 	       }
         } 
	fclose(fp);
      }
     else
     {
         LOGD("open favourit file failed or favourite file is empty\n"); 
         return -1;
     }
    FUNCTION_END
    return 0;
}


//判断文件是否为空
static int is_favouritFile_null(const char *path)
{
    FILE *fp = NULL;
    if (NULL == (fp = fopen(path, "r")))
    {
       	return GV_FAIL;
    }

    if (fgetc(fp) == EOF)
    {
       fclose(fp);
       return  GV_ERR;
    }
    else{
        fclose(fp);
	return  GV_OK;
    }
}

int operate_is_favouriteFile_empty(void)
{
	return is_favouritFile_null(FILE_FAVOURITE_PATH);
}

int add_mediapath_favourite(void)
{
        FUNCTION_BEGIN
        FILE *fp = NULL;
	char buff[256] = {0};
	char *mediapath = NULL;
	PT_Node tmp = NULL;
        bool appendFlag = false;
	//获得文件路径
    if(NULL != music_headnode)
	{
		tmp = list_entry(music_headnode, T_Node, list);
        mediapath = tmp->fullname;
    }
	else if(NULL != story_headnode)
	{
		tmp = list_entry(story_headnode, T_Node, list);
        mediapath = tmp->fullname;
    }
	else if(NULL != sinology_headnode)
	{
		tmp = list_entry(sinology_headnode, T_Node, list);
        mediapath = tmp->fullname;                                  
    }else{
        mediapath = NULL;
		return 1;
    }

    LOGD("favourtie path need to add is %s\n",mediapath);

    if(((fp=fopen(FILE_FAVOURITE_PATH,"r")) != NULL) && (fgetc(fp) != EOF)){
           //移动文件指针到开头
           fseek(fp,0L,SEEK_SET);
            while(!feof(fp)){
                if(fgets(buff,sizeof(buff),fp) != NULL)
                {
                    int j = strlen(buff);
                    if(buff[j-1] == 0x0a)
                    {
                        buff[j-1] = 0;
                    }

                    if(strcmp(mediapath, buff) != 0)
                    {
                        appendFlag=true;
                        continue;
                    }else{
                        LOGD("this file have existed in the favourite.dat");
                        appendFlag = false;
                        break;
                    }
                }
            }
            fclose(fp);
        }else{
            appendFlag = true;
        }

       LOGE("*************************************appendFlag:%d\n",appendFlag);
       if(appendFlag == true)
        {
            fp = NULL;
            if(fp == NULL)
            {
                fp = fopen(FILE_FAVOURITE_PATH,"a+");
                if(fp == NULL)
                {
                    LOGE("---open write favourite.txt fail\n");
                    return 1;
                }

                if(fputs(mediapath,fp) == EOF)
                {
                    fclose(fp);
                    LOGE("---fputs favourite.txt fail\n");
                    return 1;
                }
               if(fputs("\n",fp) == EOF){
                    fclose(fp);
                    LOGE("---fputs favourite.txt fail\n");
                    return 1;
                }

                fclose(fp);
                system("sync");
                fp=NULL;


                int length = strlen(mediapath);
                PT_Node new = (PT_Node)malloc(sizeof(T_Node));
                new->unread = 0;
                new->fullname = (char*)malloc(length+1);
                memset(buff, 0, sizeof(buff));
                strcpy(buff,mediapath);
                
                strncpy(new->fullname, buff, length + 1);            
                
                LOGE("*************************************************from favourite file to buffer:%s\n",buff);
                LOGE("*************************************************add favourite doublist new->fullname:%s\n",new->fullname);
                list_add_tail(&(new->list), gPlayListHead[3]);
                LCTX.is_favourite_empty = 0;
            }
 
     }

    FUNCTION_END
	return 0;
}


int del_mediapath_favourite(void)
{
        FUNCTION_BEGIN
	int ret = 0;
	char buffer[MAXLEN] = {0};
	char sedbuffer[MAXLEN] = {0};
	struct list_head * tmp;
	PT_Node PT_tmp;
	
    if (NULL == favourite_headnode){
        LOGD("no media content is playing!");
        return DEL_FAIL;
    }
	else
	{
		//printf("++++++++++del a node\n");
		tmp = favourite_headnode->prev;
		//剩下两个节点删除一个节点
		if ((favourite_headnode->next->next->next == favourite_headnode) && (favourite_headnode->prev == gPlayListHead[3]))
		{
			//printf("left two and del first\n");
			favourite_headnode = gPlayListHead[3]->prev;
			ret = DEL_SUCCESS;
		}
		else if((favourite_headnode->next->next->next == favourite_headnode) && (favourite_headnode->next == gPlayListHead[3]))
		{
			//printf("left two and del second\n");
			favourite_headnode = gPlayListHead[3]->next;
			ret = DEL_SUCCESS;
		}
		else if (favourite_headnode->next->next == favourite_headnode)//剩下一个节点
		{
			//printf("left only one\n");
			favourite_headnode = NULL;
                        LCTX.is_favourite_empty = 1;
			ret = ADD_EMPTY;
		}
		else if (favourite_headnode->next == gPlayListHead[3])//剩下不止一个节点，最后一个节点是头结点
		{
			favourite_headnode = favourite_headnode->next->next;
			ret = DEL_SUCCESS;
		}
		else
		{
			favourite_headnode = favourite_headnode->next;//剩下不止一个节点，最后一个不是头结点
			ret = DEL_SUCCESS;
		}
		PT_tmp = list_entry(tmp->next, T_Node, list);
		strncpy(buffer, PT_tmp->fullname, strlen(PT_tmp->fullname));
                LOGD("del path: %s\n", buffer);
		list_del(tmp->next);
		free(PT_tmp->fullname);
		PT_tmp->fullname = NULL;
		free(PT_tmp);
		PT_tmp = NULL;

        //删除文件内容
		memset(sedbuffer, 0, MAXLEN);
		sprintf(sedbuffer, "sed -i \"/%s/\"d", (buffer + sizeof(MEM_ROOT_PATH)-1 + strlen(playDir[0])+1));
        	strcat(sedbuffer, " ");
         	strcat(sedbuffer, FILE_FAVOURITE_PATH);
      	        system(sedbuffer);
                system("sync");
                memset(sedbuffer,0,sizeof(sedbuffer));
      	        LOGD("file record is remove from favourite.dat");
    }
    FUNCTION_END
    return ret;        
}


int operate_mediapath_favourite(void)   
{    
    int ret = 1;

    if (NULL != music_headnode || NULL != story_headnode || NULL != sinology_headnode)
	{               
        ret = add_mediapath_favourite(); 
    }else{ 
           if ((is_favouritFile_null(FILE_FAVOURITE_PATH) == GV_ERR) || (is_favouritFile_null(FILE_FAVOURITE_PATH) == GV_FAIL))
           {  //提示音           
              LOGD("favourite.dat list has empty or favourite.dat not exist");
              return 4;
           }
           else
           {
               ret = del_mediapath_favourite();
           } 
    } 
    return ret;        
}


int get_player_media_type(void){
    return LCTX.player_status_ctx.player_mediatype;
}
