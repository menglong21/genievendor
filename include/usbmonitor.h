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
#ifndef _USB_MONITOR_H_
#define _USB_MONITOR_H_
	
#ifdef __cplusplus  
	extern "C" {  
#endif  
	
#undef EXTERN
#ifdef _USB_MONITOR_IMPL_
#define EXTERN
#else
#define EXTERN extern
#endif
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>	
#include "genievendor.h"
#include "mediamanager.h"
#include "logic.h"

extern pthread_mutex_t usbpthreadlock;
extern pthread_cond_t  usbpthreadruncond;

extern int usb_monitor_start(pthread_t * thread);			
extern char *linecpy(char *strDest, const char *strSrc, int bufSize);
extern int readUSBFileNode(const char *dev_node,char *buffer,unsigned len);
	 	 		
#ifdef __cplusplus  
	}  
#endif  
	
#endif /*_USB_MONITOR_H_*/

