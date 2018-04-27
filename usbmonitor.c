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
/****************************************************************
*模块名称 USB插入拔出事件监测

*模块功能	T卡mount到PC作为U盘使用

****************************************************************/
#define _BATTERY_MONITOR_IMPL_


#include "usbmonitor.h"
#include "keyevent_reader.h"
pthread_mutex_t usbpthreadlock;
pthread_cond_t  usbpthreadruncond;


char *linecpy(char *strDest, const char *strSrc, int bufSize)
{
    if(strDest==NULL || strSrc==NULL || bufSize<0)
    {
        return NULL;
    }

    char *pDest = (char *)strDest;
    char *pSrc =  (char *)strSrc;

    while(*pSrc!=0 && *pSrc!=0x0d && *pSrc!=0x0a && --bufSize)
    {
        *pDest++ = *pSrc++;
    }
    *pDest = 0;
    return strDest ;
}


int readUSBFileNode(const char *dev_node,char *buffer,unsigned len)
{
	FILE *fp;
	int ret = OK;
    char buf[32] = {0};

	fp = fopen(dev_node,"r");
	if (fp == NULL)
    {
		LOGE("[r]fopen %s fail:(%s)\n",dev_node,strerror(errno));
		return ERROR;
	}

    buffer[0] = 0;
    if(fgets(buf,sizeof(buf),fp) != NULL)
    {
        linecpy(buffer,buf,0);
    }
	else
    {
		LOGE("[r]fgets %s fail:(%s)\n", dev_node, strerror(errno));
		ret = ERROR;
	}

	if (fclose(fp) < 0)
    {
		LOGE("[r]fclose %s fail:(%s)\n", dev_node, strerror(errno));
	}

	return OK;
}

/*USB 监控线程*/
void* _USB_monitor_thread(void* arg){
	FUNCTION_BEGIN	
	char value[32] = {0};	
    USB_STATUS_E status=GV_USB_DISCONNECTED;
	//usleep(30 * 1000 * 1000);//sleep 30S when booting
	while(1){
		if(LCTX.is_poweron_keypress_lock){
                pthread_mutex_lock(&usbpthreadlock);
                pthread_cond_wait(&usbpthreadruncond,&usbpthreadlock);
                pthread_mutex_unlock(&usbpthreadlock);
		}
		
		if(readUSBFileNode("/sys/class/android_usb/android0/state",value,sizeof(value)) == OK)
        {
            if(strcmp(value,"CONFIGURED") == 0){
                if(LCTX.player_usb_status_ctx.UDiskPlugInFlag==FALSE)
                {
                    pthread_mutex_lock(&sdscanlock);
                    LCTX.player_usb_status_ctx.UDiskPlugInFlag=TRUE;
					system("sync");
            		system("umount -vl /mnt/sdcard/");
            		system("echo /dev/mmcblk0p1 > /sys/class/android_usb/android0/f_mass_storage/lun/file");
                    LCTX.player_sd_status_ctx.SDcardRefreshFlag=FALSE;
                    status=GV_SDCARD_UMOUNT;
                    logic_handle_sd_status_change(&LCTX,status);
                    status = GV_USB_CONNECTED;
                     
                    logic_handle_usb_status_change(&LCTX,status);
                    pthread_cond_signal(&sdscancond);     
                    pthread_mutex_unlock(&sdscanlock);    
                    LOGD("UDiskFlag = TRUE");
                }
            }else{
                if(LCTX.player_usb_status_ctx.UDiskPlugInFlag==TRUE)
                {
                    pthread_mutex_lock(&sdscanlock);
                    LCTX.player_usb_status_ctx.UDiskPlugInFlag = FALSE;
					system("sync");
            		system("mount -t vfat -o codepage=936,iocharset=utf8 /dev/mmcblk0p1 /mnt/sdcard");	
                    LCTX.player_sd_status_ctx.SDcardRefreshFlag=TRUE;
                    status = GV_USB_DISCONNECTED;
                    logic_handle_usb_status_change(&LCTX,status);
                    pthread_cond_signal(&sdscancond);          
                    pthread_mutex_unlock(&sdscanlock);
					LOGD("UDiskFlag = FALSE");
                }else if(LCTX.player_usb_status_ctx.UDiskPlugInFlag==FALSE){
                    if(LCTX.player_sd_status_ctx.SDcardRefreshFlag==TRUE){
                        pthread_mutex_lock(&sdscanlock);
                        pthread_cond_signal(&sdscancond);     
                        pthread_mutex_unlock(&sdscanlock);
                        
                    }
                }
           }
       }	
       usleep(1 * 1000 * 1000);
	}
	FUNCTION_END
}


/*启动USB Plug into/Out 监控线程*/
int usb_monitor_start(pthread_t* thread){
    FUNCTION_BEGIN
    int ret;
    LCTX.player_usb_status_ctx.UDiskPlugInFlag=FALSE;    
    LCTX.player_usb_status_ctx.usb_status=GV_USB_DISCONNECTED;
    ret = pthread_create(thread, NULL, _USB_monitor_thread, NULL);
    if(0 == ret){
        LOGD("usb monitor thread created!");
        FUNCTION_END
        return GV_OK;
    } else {
        LOGE("usb monitor thread create failed!");
        FUNCTION_END
        return GV_FAIL;
    }
}
