/*****************************************************************
*
* 
*****************************************************************/

/*****************************************************************
* 模块名称
*
* GenieVendor socket读写模块
*
* 模块功能
*
* 1、监听socket接口，读取GenieEngine发来的信息
* 2、将制定信息通过socket发送给GenieEngine
*
*****************************************************************/

#define _SOCKET_MANAGER_IMPL_

#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "cJSON.h"
#include "socketmanager.h"
#include "jsonparser.h"
#include "logic.h"
#include "libaglog.h"
#undef LOG_TAG
#define LOG_TAG "GVSocketManager"


SOCKET_CONTEXT_T SCTX;

/*初始化发送接收socket并启动监听线程*/
static int _socket_init(SOCKET_CONTEXT_T * ctx){
    FUNCTION_BEGIN

    int ret;
    struct sockaddr_un * server_addr = &ctx->recv_sock_addr;
    struct sockaddr_un * client_addr = &ctx->send_sock_addr;
    memset(server_addr, 0, sizeof(struct sockaddr_un));
    memset(client_addr, 0, sizeof(struct sockaddr_un));
    
 
    /*初始化socket addr*/
    strcpy(server_addr->sun_path, SOCK_RECV);
    strcpy(client_addr->sun_path, SOCK_SEND);
    server_addr->sun_family = AF_UNIX;
    client_addr->sun_family = AF_UNIX;
    unlink(ctx->recv_sock_addr.sun_path);
    
    /*打开接收socket*/
    ctx->recv_sockfd = socket(PF_UNIX, SOCK_DGRAM, 0);
    if(ctx->recv_sockfd < 0){
        LOGE("Cannot create receive socket!");
        goto error;
    }

    /*绑定接收socket*/
    ret = bind(ctx->recv_sockfd, (struct sockaddr*)server_addr, sizeof(ctx->recv_sock_addr));
    if(ret != 0) {
        LOGE("Cannot bind receive socket!");
        goto error;
    }

    /*打开发送socket*/
    ctx->send_sockfd = socket(PF_UNIX, SOCK_DGRAM, 0);
    if(ctx->send_sockfd < 0){
        LOGE("Cannot create send socket!");
        goto error;
    }
    
    LOGD("socket init succeed.");
    ctx->is_send_sock_ready = GV_CTX_READY;
    FUNCTION_END

    return GV_OK;
    
    error:
    LOGE("Destroy socket");
    close(ctx->recv_sockfd);
    close(ctx->send_sockfd);
    unlink(SOCK_RECV);
    
    FUNCTION_END
    return GV_FAIL;

}


void* _socket_recv_thread(void * arg){

    FUNCTION_BEGIN

    char cmd[VENDOR_CMD_LENGTH_MAX+1] = {0};
    char send[VENDOR_CMD_LENGTH_MAX+1] = {0};
    cJSON * vendor_cmd;
    int ret;
    
    //循环监听
    while(1){
        
        memset(cmd, 0, sizeof(cmd));
        memset(send, 0, sizeof(send));
        //读socket
        recv(SCTX.recv_sockfd, cmd, sizeof(cmd), 0);
        LOGD("cmd received:\n%s",cmd);
        //cJSON_Delete在下方
        vendor_cmd = cJSON_Parse(cmd);
        if (NULL == vendor_cmd)
        {
            cJSON_Delete(vendor_cmd);
            LOGE("JSON parse error, vendor_cmd is NULL");
            continue;
        }

        //解析和处理消息
        ret = handle_engine_json(vendor_cmd);
        cJSON_Delete(vendor_cmd);
        if(ret == GV_OK){
            LOGD("successfully handled a json notify.");
        } else {
            LOGE("handle a json notify failed!");
        }
    }

    FUNCTION_END
    
}


/*发送消息给GenieEngine*/
int socket_send(SOCKET_CONTEXT_T * ctx, char * buffer, int len)
{
    FUNCTION_BEGIN

    int ret;
    
    if(GV_CTX_READY != ctx->is_send_sock_ready){
        LOGE("Socket has not been inited yet while sending a msg!");
        return GV_FAIL;
    }
    LOGD("cmd send:\n%s", buffer);
    ret = sendto(ctx->send_sockfd,
                  buffer,
                  len,
                  0,
                  (struct sockaddr *)&ctx->send_sock_addr,
                  sizeof(struct sockaddr_un));
    if(ret < 0 ){
        LOGE("send to engine failed! ret=%d", ret);
    } else {
        LOGD("send to engine with %d bytes.", ret);
    }
    FUNCTION_END
    return ret;
}

int socket_start(pthread_t* thread){
    FUNCTION_BEGIN

    int ret;

	memset(&SCTX,0,sizeof(SCTX));
    if(GV_OK != _socket_init(&SCTX)){
        LOGE("socket init failed, exit...");
        goto exit;
    };
    
    ret = pthread_create(thread, NULL, _socket_recv_thread, NULL);

    if(0 == ret){
        return GV_OK;
    }
    
    exit:
    
    close(SCTX.recv_sockfd);
    close(SCTX.send_sockfd);
    unlink(SOCK_RECV);
    FUNCTION_END
    return GV_FAIL;

}

