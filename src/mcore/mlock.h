#pragma once

#ifndef __mlock_h__
#define __mlock_h__


#ifdef __cplusplus
extern "C" {
#endif


#if defined(_MSC_VER)

    typedef CRITICAL_SECTION    mlock_obj;

#define mlock_simple_init(pObj)             InitializeCriticalSection(&pObj)
#define mlock_simple_wait(pObj)             EnterCriticalSection(&pObj);
#define mlock_simple_release(pObj)          LeaveCriticalSection(&pObj);

#else
#include <pthread.h>
    typedef pthread_mutex_t     mlock_obj;

#define mlock_simple_init(pObj)             pthread_mutex_init(&pObj, NULL)
#define mlock_simple_wait(pObj)             pthread_mutex_lock(&pObj);
#define mlock_simple_release(pObj)          pthread_mutex_unlock(&pObj);


#endif

#ifdef __cplusplus
}
#endif

#endif
