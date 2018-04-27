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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <asm/types.h>
#include <dlfcn.h>
#ifndef __ANDROID__
#include <execinfo.h>
#endif

#define _BACKTRACE_IMPL_
#include "backtrace.h"

#include "libaglog.h"
#undef LOG_TAG
#define LOG_TAG "GVBackTrace"

void dump(int signo)
{
    void *buffer[100] = {0};
    size_t size;
    char **strings = NULL;
    size_t i = 0;

    size = backtrace(buffer, 100);
    fprintf(stdout, "Obtained %zd stack frames.nm\n", size);
    strings = backtrace_symbols(buffer, size);
    if (strings == NULL) {
        perror("backtrace_symbols.");
        exit(0);
    }

    for (i = 0; i < size; i++) {
        fprintf(stdout, "%s\n", strings[i]);
    }

    free(strings);
    strings = NULL;

    exit(0);
}

void backtrace_init(void)
{
    LOGD("backtrace inited.");

    if(signal(SIGSEGV, dump) == SIG_ERR) {
        perror("can't catch SIGSEGV");
    }
}
