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

#ifndef _SOCKET_MANAGER_H_
#define _SOCKET_MANAGER_H_

#ifdef __cplusplus  
extern "C" {  
#endif  

#undef EXTERN
#ifdef _SOCKET_MANAGER_IMPL_
#define EXTERN
#else
#define EXTERN extern
#endif

#include <sys/socket.h>
#include <sys/un.h>
#include "genievendor.h"

/*socket上下文结构*/
typedef struct
{
	struct sockaddr_un recv_sock_addr;
	struct sockaddr_un send_sock_addr;
	int recv_sockfd;
	int send_sockfd;
    unsigned short is_send_sock_ready;
} SOCKET_CONTEXT_T;
#define GV_CTX_READY    (1)

/*socket路径*/
#define SOCK_RECV "/tmp/engine2vendor.sock"
#define SOCK_SEND "/tmp/vendor2engine.sock"

extern SOCKET_CONTEXT_T SCTX;

EXTERN int socket_send(SOCKET_CONTEXT_T * ctx, char * buffer, int len);
EXTERN int socket_start();

#ifdef __cplusplus  
}  
#endif  

#endif /*_SOCKET_MANAGER_H_*/
