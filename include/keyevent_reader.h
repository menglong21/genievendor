#ifndef _KEYEVENT_READER_H_
#define _KEYEVENT_READER_H_

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

#undef EXTERN
#ifdef _KEYEVENT_READER_IMPL_
#define EXTERN
#else
#define EXTERN extern
#endif /*_KEYEVENT_READER_IMPL_*/

#include <pthread.h>

extern pthread_mutex_t keypthreadlock;
extern pthread_cond_t  keypthreadruncond;

EXTERN int keyevent_reader_init(pthread_t * thread);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*_KEYEVENT_READER_H_*/
