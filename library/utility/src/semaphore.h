/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: semaphore.h
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/
//无名(内存)信号量，可以用于线程（多用于）/进程间同步（信号量要放在共享内存中） 

#ifndef __UTILITY_SEMAPHORE_H__
#define __UTILITY_SEMAPHORE_H__

#ifdef WIN32
#else
#include <semaphore.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#endif

#include "utility.h"
#include "datetime.h"

namespace utility {

class COMMON_API semaphore {
public:
	semaphore(long init_count = 0, long max_count = 1);

	~semaphore();

	bool wait() const;
	bool try_wait() const;
	bool wait_timed(const date_time&) const;
	bool post(int count = 1) const;

private:
#if defined(WIN32)
	mutable HANDLE sem_;
#else
	mutable sem_t sem_;
#endif
};

}
#endif
