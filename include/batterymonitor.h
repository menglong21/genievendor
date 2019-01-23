/*****************************************************************
*
*
*****************************************************************/

#ifndef _BATTERY_MONITOR_H_
#define _BATTERY_MONITOR_H_

#ifdef __cplusplus  
extern "C" {  
#endif  

#undef EXTERN
#ifdef _BATTERY_MONITOR_IMPL_
#define EXTERN
#else
#define EXTERN extern
#endif

extern float battery_voltage;
extern int battery_percent;

#include <pthread.h>
extern pthread_mutex_t batterypthreadlock;
extern pthread_cond_t  batterypthreadruncond;

EXTERN int battery_monitor_start(pthread_t * thread);

#ifdef __cplusplus  
}  
#endif  

#endif /*_BATTERY_MONITOR_H_*/
