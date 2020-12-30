/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: utility.h
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#ifndef __UTILITY_UTILITY_H__
#define __UTILITY_UTILITY_H__

#include <string>
#include <stdlib.h>

#ifdef WIN32
#ifdef COMMON_EXPORT
#define COMMON_API  __declspec(dllexport)
#else 
#define COMMON_API  __declspec(dllimport)
#endif  //end COMMON_EXPORT

#else	//LINUX平台，定义导出宏为空
#define COMMON_API
#endif

#ifdef WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#define q_sleep(milliseconds) Sleep(milliseconds)
#define get_th_id() GetCurrentThreadId()
#else
#include <pthread.h>
#include <semaphore.h>
#include <sys/syscall.h>
#include <unistd.h>
#define q_sleep(milliseconds) usleep(milliseconds*1000)
#define get_th_id() syscall(SYS_gettid)
#endif

#ifdef WIN32
#include <math.h>
#define fmax max
#define fmin min
#define fabs abs
#endif

/* atomic operations defines */
#ifdef WIN32
#define base_fetch_and_inc(x) InterlockedExchangeAdd((x), 1)
#define base_fetch_and_dec(x) InterlockedExchangeAdd((x), -1)
#define base_fetch_and_add(x, c) InterlockedExchangeAdd((x), c)
#define base_fetch_and_sub(x, c) InterlockedExchangeAdd((x), -(c))
#define base_compare_and_exchange(des, cmp, exch) InterlockedCompareExchange((des), (exch), (cmp))
#define base_exchange(des, exch) InterlockedExchange((des), (exch))
#else
#define base_fetch_and_inc(x) __sync_fetch_and_add((x), 1)
#define base_fetch_and_dec(x) __sync_fetch_and_sub((x), 1)
#define base_fetch_and_add(x, c) __sync_fetch_and_add((x), c)
#define base_fetch_and_sub(x, c) __sync_fetch_and_sub((x), c)
#define base_compare_and_exchange(des, cmp, exch) __sync_val_compare_and_swap((des), (cmp), (exch))
#define base_exchange(des, exch) __sync_lock_test_and_set((des), (exch))
#endif
#endif
