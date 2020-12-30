/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: ssemaphore.h
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/
//有名信号量，可以用于进程（多用于）/线程间同步 

#ifndef __UTILITY_SSEMAPHORE_H__
#define __UTILITY_SSEMAPHORE_H__

#include <string>

#ifdef WIN32
#include <Windows.h>
#include <tchar.h>
#else
#include <semaphore.h>
#include <fcntl.h>           /* 为了 O_* 常量 */
#include <sys/stat.h>        /* 为了模式常量 */
#endif

#include "utility.h"

namespace utility {

	class COMMON_API ssemaphore
	{
	public:
		ssemaphore(long initial = 0, const char *sem_name = NULL);
#if defined(WIN32)
		bool wait(int millisecond_=INFINITE);
		bool release(int counter_=1);
#else
		bool wait(int millisecond_=-1);
		bool release();
#endif
		bool try_wait();
		~ssemaphore();
	private:

#if defined(WIN32)
		HANDLE sem_;
#else
		sem_t* sem_;
		std::string sem_name;
#endif

	};

}
#endif
