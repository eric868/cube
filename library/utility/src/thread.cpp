/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: thread.cpp
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#include "thread.h"

namespace utility {

	thread::thread()
		: m_state(STOPED)
	{
		m_lock = new spinlock();
	}

	thread::~thread()
	{
		delete m_lock;
	}

	void thread::start() {
		if (get_state() == RUNING) { return; }

		m_state = RUNING;
#ifdef WIN32
		int ret = _beginthread(thread_proc, 1000, this);
		if (ret < 0) {
			throw runtime_error("create thread error");
			set_state(STOPED);
			return;
		}
#else
		int ret = pthread_create(&m_tid, 0, thread_proc, this);
		if(ret) {
			throw runtime_error("create thread error");
			set_state(STOPED);
			return;
		}
#endif 
	}

	void thread::join() {
#ifdef WIN32
		while (get_state() != STOPED) {
			q_sleep(1000);
		}
#else
		pthread_join(m_tid, 0);
#endif
	}

	void thread::stop() {
		set_state(REQUEST_STOP);
	}

	bool thread::is_runing() {
		return get_state() == RUNING;
	}

	void thread::set_state(thread_state state) {
		scope_spinlock locker(m_lock);
		m_state = state;
	}

	thread::thread_state thread::get_state() {
		scope_spinlock locker(m_lock);
		return m_state;
	}

}
