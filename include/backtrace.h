/*****************************************************************
* 
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
