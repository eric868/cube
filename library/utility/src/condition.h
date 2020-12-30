/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: condition.h
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#ifndef __UTILITY_CONDITION_H__
#define __UTILITY_CONDITION_H__ 

#include <assert.h> 
#if defined WIN32
#include <windows.h>  
#else  
#include <pthread.h>  
#endif  

#include "mutex.h"

namespace utility {

class COMMON_API condition
{
public:
    condition();
    virtual ~condition();

    bool signal();
    bool broadcast();

    //release lock, wait for signal or interrupt, lock and wake  
	bool wait(mutex *mutex);
	bool wait_time(mutex *mutex, unsigned s, unsigned ms = 0);
private:
#if defined WIN32
	CONDITION_VARIABLE m_cond;
#else
	pthread_cond_t m_cond;
#endif
};
}
#endif
