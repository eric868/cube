/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: thread.cpp
Version: 1.0
Date: 2016.1.13

History:
base     2016.1.13   1.0     Create
******************************************************************************/

#include "thread.h"
#include "trace.h"
#ifdef _MSC_VER
#else
#include <sys/sysctl.h>
#include <unistd.h>
#endif

namespace base {

thread::thread()
	: thread_(NULL)
{
}

thread::~thread()
{
	join();
}

void thread::create(thread_proc_t func, void* args)
{
#ifdef _MSC_VER
	thread_ = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)func, args, 0, NULL);
#else
	pthread_create(&thread_, NULL, (void*(*)(void*))func, args);
#endif
}

void thread::join()
{
#ifdef _MSC_VER
	if (thread_) {
		WaitForSingleObject(thread_, INFINITE);
		CloseHandle(thread_);
		thread_ = NULL;
	}
#else
	if (thread_) {
		void* result;
		pthread_join(thread_, &result);
		thread_ = NULL;
	}
#endif
}

void thread::set_priority(THREAD_PRIORITY pri)
{
#ifdef _MSC_VER
	SetThreadPriority(thread_, pri == THREAD_PRI_LOW ? THREAD_PRIORITY_IDLE : THREAD_PRIORITY_HIGHEST);
#else

#endif
}

void thread::detach()
{
#ifdef _MSC_VER
	if (thread_) {
		CloseHandle(thread_);
		thread_ = NULL;
	}
#else
	pthread_detach(thread_);
#endif
}

void thread::sleep(unsigned int milli_seconds)
{
#ifdef _MSC_VER
	Sleep(milli_seconds);
#else
	/*struct timespec ts;
	ts.tv_sec = milli_seconds / 1000;
	ts.tv_nsec = (milli_seconds % 1000) * 1000000;
	nanosleep(&ts, NULL);*/
    usleep(milli_seconds * 1000);
#endif
}

long thread::curr_thread_id()
{
#ifdef _MSC_VER
	return GetCurrentThreadId();
#else
	return (long)pthread_self();
#endif
}

int thread::number_of_processors()
{
#ifdef _MSC_VER
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	return sysinfo.dwNumberOfProcessors;
#else
	int ncpus = (int)sysconf(_SC_NPROCESSORS_CONF);
	return ncpus;
#endif
}

/****************************************************************************/

process_thread::process_thread()
	: thread_(NULL)
	, is_running_(false)
{

}

process_thread::~process_thread()
{

}

void process_thread::run()
{

}

void process_thread::start()
{
#ifdef _MSC_VER
	thread_ = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)_thread_proc, this, 0, NULL);
	if (thread_ == NULL) {
		TRACE_ERROR("base", 0, "fail to create process_thread");
	}
	else {
		set_priority(base::THREAD_PRI_NORMAL);
	}
#else
	int ret = pthread_create(&thread_, NULL, _thread_proc, this);
	if (ret != 0) {
		TRACE_ERROR("base", 0, "fail to create process_thread");
	}
#endif
}

void process_thread::stop()
{
	if (!is_running_ && thread_ != 0) {
		process_thread::sleep(10);
	}

	if (is_running_) {
		is_running_ = false;

#ifdef _MSC_VER
		WaitForSingleObject(thread_, INFINITE);
		CloseHandle(thread_);
#else
		pthread_join(thread_, NULL);
#endif
		thread_ = NULL;
	}
}

void process_thread::set_priority(THREAD_PRIORITY pri)
{
#ifdef _MSC_VER
	SetThreadPriority(thread_, pri == THREAD_PRI_LOW ? THREAD_PRIORITY_IDLE : THREAD_PRIORITY_HIGHEST);
#else

#endif
}

bool process_thread::wait(long milli_seconds)
{
	return true;
}

void process_thread::release()
{

}

bool process_thread::is_running()
{
	return is_running_;
}

void process_thread::set_running(bool is_running)
{
	is_running_ = is_running;
}

void process_thread::sleep(unsigned int milli_seconds)
{
#ifdef _MSC_VER
	Sleep(milli_seconds);
#else
	/*struct timespec ts;
	ts.tv_sec = milli_seconds / 1000;
	ts.tv_nsec = (milli_seconds % 1000) * 1000000;
    nanosleep(&ts, NULL);*/
    usleep(milli_seconds * 1000);
#endif
}

#ifdef _MSC_VER
unsigned int _thread_proc(void* param)
{
	try {
		process_thread* p = (process_thread*)param;
		p->set_running(true);
		p->run();
	}
	catch (...)
	{
	}

	return 0;
}
#else
void* _thread_proc(void* param)
{
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

	try {
		process_thread* p = (process_thread*)param;
		p->set_running(true);
		p->run();
	}
	catch (...)
	{
	}

	pthread_exit(NULL);
	return NULL;
}
#endif

} // end of namespace base
