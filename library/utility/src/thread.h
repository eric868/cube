/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: thread.h
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#ifndef __UTILITY_THREAD_H__
#define __UTILITY_THREAD_H__

#include <stdexcept>
#include <iostream>
#include "utility.h"
#include "spinLock.h"

#ifdef WIN32
#include <windows.h>
#include <process.h>
#else
#include <pthread.h>
#endif

using namespace std;

namespace utility {

	class COMMON_API thread {
	public:
		enum thread_state {
			RUNING,
			REQUEST_STOP,
			STOPED
		};
	public:	
		thread();
		virtual ~thread();

		void start();
		void join();
		void stop();
		bool is_runing();
		thread_state get_state();

		virtual void run() = 0;
	private:
#ifdef WIN32
		static void thread_proc(void* arg)
#else
		static void* thread_proc(void* arg)
#endif
		{
			thread* thd = (thread*)arg;
			thd->run();
			thd->set_state(STOPED);
		}

		void set_state(thread_state state);

	private:
		mutable thread_state m_state;
		spinlock* m_lock;

#ifdef WIN32
#else
		pthread_t m_tid;
#endif

	};

}
#endif
