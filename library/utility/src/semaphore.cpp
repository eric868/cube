/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: semaphore.cpp
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#include "semaphore.h"

namespace utility {

semaphore::semaphore(long init_count, long max_count)
{
#if defined(WIN32)
	sem_ = CreateSemaphore(0, init_count, max_count, 0);
#else
	//第二个参数为0,则在同一个线程间共享
	sem_init(&sem_, 0 , init_count);
#endif
}

semaphore::~semaphore()
{
#if defined(WIN32)
	CloseHandle(sem_);
#elif __GNUG__
	sem_destroy(&sem_);
#endif
}

bool semaphore::wait() const
{
#if defined(WIN32)
	int rc = WaitForSingleObject(sem_, INFINITE);
	if (rc != WAIT_OBJECT_0) {
		return false;
	}
#elif __GNUG__
	sem_wait(&sem_);
#endif

	return true;
}

bool semaphore::try_wait() const
{
#ifdef WIN32
	return (WaitForSingleObject(sem_, 1) != WAIT_TIMEOUT);
#else
	return (sem_trywait(&sem_) == 0);
#endif
}

bool semaphore::wait_timed(const date_time& time) const
{
	return false;
}

//用于以原子操作的方式将信号量的值加1
bool semaphore::post(int count/* = 1 */) const
{
#if defined(WIN32)    
	ReleaseSemaphore(sem_, count, 0);
#else
	sem_post(&sem_);
#endif

	return true;
}

}
