
#ifndef __mthread_h__
#define __mthread_h__

#if defined(__cplusplus)
extern "C" {
#endif/* defined(__cplusplus) */

#if defined(_MSC_VER)
#define mthread_sleep(ms)       Sleep(ms)
#else
#define mthread_sleep(ms)       usleep(ms * 1000)
#endif

#if defined(__cplusplus)
}
#endif/* defined(__cplusplus) */

#endif
