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


#ifndef _BACKTRACE_H_
#define _BACKTRACE_H_

#ifdef __cplusplus  
extern "C" {  
#endif /*__cplusplus*/

#undef EXTERN
#ifndef _BACKTRACE_IMPL_
#define EXTERN
#else
#define EXTERN extern
#endif /*_BACKTRACE_IMPL_*/

EXTERN void backtrace_init(void);

#ifdef __cplusplus  
}
#endif /*__cplusplus*/  

#endif /*_BACKTRACE_H_*/
