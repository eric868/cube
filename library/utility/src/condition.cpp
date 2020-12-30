/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: condition.cpp
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#include "condition.h"

namespace utility {

condition::condition()
{
#if defined WIN32
    ::InitializeConditionVariable(&m_cond);
#else
    int rs = pthread_cond_init(&m_cond, NULL);
    assert(0 == rs);
#endif
}

condition::~condition()
{
#if defined WIN32
    /* no need to destroy in windows */
#else
    int rs = pthread_cond_destroy(&m_cond);
    assert(0 == rs);
#endif
}

bool condition::signal()
{
#if defined WIN32
    ::WakeConditionVariable(&m_cond);
    return true;
#else
    return 0 == pthread_cond_signal(&m_cond);
#endif
}

bool condition::broadcast()
{
#if defined WIN32
    ::WakeAllConditionVariable(&m_cond);
    return true;
#else
    return 0 == pthread_cond_broadcast(&m_cond);
#endif
}

//release lock, wait for signal or interrupt, lock and wake
bool condition::wait(mutex *mutex)
{
    assert(NULL != mutex);
#if defined WIN32
    return SleepConditionVariableCS(&m_cond, mutex->get_mutex(), INFINITE);
#else
	return 0 == pthread_cond_wait(&m_cond, mutex->get_mutex());
#endif
}

bool condition::wait_time(mutex *mutex, unsigned s, unsigned ms/* = 0*/)
{
    assert(NULL != mutex);
#if defined WIN32
    DWORD dwMilliseconds = s * 1000 + ms;
    return SleepConditionVariableCS(&m_cond, mutex->get_mutex(), dwMilliseconds);
#else
    struct timespec abstime;
    clock_gettime(CLOCK_REALTIME, &abstime);
    abstime.tv_sec += s;
    abstime.tv_nsec += ((long)ms) * 1000 * 1000;
	return 0 == pthread_cond_timedwait(&m_cond, mutex->get_mutex(), &abstime);
#endif
}

}
