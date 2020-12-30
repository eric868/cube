/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: spinlock.h
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

//自旋锁：消耗很少的资源来建立锁,随后当线程被阻塞时,
//它就会一直重复检查看锁是否可用了，
//也就是说当自旋锁处于等待状态时它会一直消耗CPU时间。
//自旋锁就主要用在临界区持锁时间非常短且CPU资源不紧张的情况下。

#ifndef __UTILITY_SPINLOCK_H__
#define __UTILITY_SPINLOCK_H__

#include <assert.h>  
#if defined(WIN32)
#include <windows.h>  
#else  
#include <pthread.h>  
#endif  

#include "utility.h"

namespace utility {

//不允许重复锁
class COMMON_API spinlock  
{  
public:  
	spinlock(int spin_count = 4000);
	virtual ~spinlock();

#if defined(WIN32)  
	CRITICAL_SECTION* get_spinlock() { return &m_critical_section; }  
#else  
	pthread_spinlock_t* get_spinlock() { return &m_spinlock; }  
#endif  

	void lock();
	bool try_lock();
	void unlock(); 
private:
#if defined(WIN32)  
	CRITICAL_SECTION m_critical_section;
#else  
	pthread_spinlock_t m_spinlock;
#endif  
};

inline void spinlock::lock()
{  
#if defined(WIN32)  
	EnterCriticalSection(&m_critical_section);  
	//assert(m_critical_section.RecursionCount == 1);
#else  
	int rs = ::pthread_spin_lock(&m_spinlock);  
	assert(0 == rs);  
#endif  
}  

inline bool spinlock::try_lock()
{  
#if defined(WIN32)  
	if(!TryEnterCriticalSection(&m_critical_section)) { return false; }

	if(m_critical_section.RecursionCount > 1) {
		LeaveCriticalSection(&m_critical_section);
		throw ("Thread locked expcetion! LeaveCriticalSection");
	}
	return true;
#else  
	return 0 == ::pthread_spin_trylock(&m_spinlock);  
#endif  
}  

inline void spinlock::unlock()
{  
#if defined(WIN32)  
	//assert(m_critical_section.RecursionCount == 1);
	LeaveCriticalSection(&m_critical_section);  
#else  
	int rs = ::pthread_spin_unlock(&m_spinlock);  
	assert(0 == rs);  
#endif  
}  

class scope_spinlock {
public:
	scope_spinlock(spinlock* spin_lock) 
		: m_spinlock(spin_lock) {
		m_spinlock->lock();
	}

	virtual ~scope_spinlock() {
		m_spinlock->unlock();
	}

private:
	spinlock* m_spinlock;
};

}
#endif
