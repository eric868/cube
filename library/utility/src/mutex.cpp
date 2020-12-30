/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: mutex.cpp
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#include "mutex.h"

namespace utility {

mutex::mutex ()
{
#if defined WIN32
    InitializeCriticalSection(&m_mutex);
#else
	int rs = pthread_mutex_init(&m_mutex, NULL);
	assert(rs == 0);
#endif
}

mutex::~mutex ()
{
#ifdef WIN32
	DeleteCriticalSection(&m_mutex);
#else
    int rs = ::pthread_mutex_destroy(&m_mutex);
    assert(0 == rs);
#endif
}

}
