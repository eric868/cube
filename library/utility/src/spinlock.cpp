/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: spinlock.cpp
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#include "spinlock.h"

namespace utility {

	spinlock::spinlock(int spin_count/* = 10000*/)
	{
#if defined(WIN32)  
		//旋转spin_count个时钟周期后进入阻塞
		InitializeCriticalSectionAndSpinCount(&m_critical_section, spin_count);
#else  
		//PTHREAD_PROCESS_PRIVATE:
		//仅初始化本自旋锁的线程所在的进程内的线程才能够使用该自旋锁。
		int rs = ::pthread_spin_init(&m_spinlock, PTHREAD_PROCESS_PRIVATE);  
		assert(0 == rs);  
#endif  
	}

	spinlock::~spinlock()
	{
#if defined(WIN32)  
		DeleteCriticalSection(&m_critical_section);
#else  
		int rs = ::pthread_spin_destroy(&m_spinlock);  
		assert(0 == rs);  
#endif  
	}

}
