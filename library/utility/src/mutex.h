/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: mutex.h
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/
/*
互斥锁：线程会从sleep（加锁）——>running（解锁），
过程中有上下文的切换，cpu的抢占，信号的发送等开销。
互斥锁用于临界区持锁时间比较长的操作，比如下面这些情况都可以考虑
1 临界区有IO操作
2 临界区代码复杂或者循环量大
3 临界区竞争非常激烈
4 单核处理器
*/

#ifndef __UTILITY_MUTEX_H__
#define __UTILITY_MUTEX_H__

#include <assert.h> 
#include "utility.h"
#ifdef WIN32
#include <windows.h>  
#else  
#include <pthread.h>  
#include <errno.h>
#endif  

namespace utility {

//不允许重复锁
class COMMON_API mutex
{
public :
    mutex ();
    ~mutex ();

    void lock();
    void unlock();
    //return true, if lock successed 
    bool trylock();

#if defined WIN32
	CRITICAL_SECTION* get_mutex() { return &m_mutex; }
#else
	pthread_mutex_t* get_mutex() { return &m_mutex; }
#endif

private:
#ifdef WIN32
	mutable CRITICAL_SECTION m_mutex;
#else
	mutable pthread_mutex_t m_mutex;
#endif
};

inline void mutex::lock()
{
#if defined WIN32
	EnterCriticalSection(&m_mutex);  
	assert(m_mutex.RecursionCount == 1);
#else
    int rs = pthread_mutex_lock(&m_mutex);
    assert(0 == rs);
#endif
}

inline void mutex::unlock()
{
#if defined WIN32
	assert(m_mutex.RecursionCount == 1);
	LeaveCriticalSection(&m_mutex);
#else
    int rs = pthread_mutex_unlock(&m_mutex);
    assert(0 == rs);
#endif
}

inline bool mutex::trylock()
{
#ifdef WIN32
	if(!TryEnterCriticalSection(&m_mutex)) { return false; }

	if(m_mutex.RecursionCount > 1) {
		LeaveCriticalSection(&m_mutex);
		throw ("Thread locked expcetion! LeaveCriticalSection");
	}
	return true;
#else
    int rs = pthread_mutex_trylock(&m_mutex);
	if(rs != 0 && rs != EBUSY) {
		throw ("Invalid mutex");
	}
    /*
	returned values :
    0:lock ok
    EBUSY:The mutex is already locked.
    EINVAL:Mutex is not an initialized mutex.
    EFAULT:Mutex is an invalid pointer.
    */
    return 0 == rs;
#endif
}

class scope_mutex {
public:
	scope_mutex(mutex* mutex) 
		: m_mutex(mutex) 
	{
		m_mutex->lock();
	}

	virtual ~scope_mutex() 
	{
		m_mutex->unlock();
	}

private:
	mutex* m_mutex;
};

}
#endif
