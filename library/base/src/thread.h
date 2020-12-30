/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: thread.h
Version: 1.0
Date: 2016.1.13

History:
base     2016.1.13   1.0     Create
******************************************************************************/

#ifndef __BASE_THREAD_H__
#define __BASE_THREAD_H__

#ifdef _MSC_VER
#include <Windows.h>
#else
#include <pthread.h>
#endif
#include "base.h"
namespace base {

enum THREAD_PRIORITY
{
	THREAD_PRI_LOW = -1,
	THREAD_PRI_NORMAL = 0,
	THREAD_PRI_HIGH = 1
};

#ifdef _MSC_VER
#define thread_proc WINAPI
#else
#define thread_proc 
#endif
class COMMON_API thread
{
public:
	typedef void (thread_proc *thread_proc_t)(void*);

public:
	thread();
	~thread();

public:
	void create(thread_proc_t func, void* args);
	void join();
	void set_priority(THREAD_PRIORITY pri);
	void detach();

public:
	static void sleep(unsigned int milli_seconds);
	static long curr_thread_id();
	static int number_of_processors();

private:
#ifdef _MSC_VER
	HANDLE thread_;
#else
	pthread_t thread_;
#endif
};

class COMMON_API process_thread
{
public:
	process_thread();
	virtual ~process_thread();

public:
	virtual void run();

	virtual void start();
	virtual void stop();

	void set_priority(THREAD_PRIORITY pri);

	bool wait(long milli_seconds = 5000);
	void release();

	bool is_running();
	void set_running(bool is_running);

public:
	static void sleep(unsigned int milli_seconds);

protected:
#ifdef _MSC_VER
	HANDLE thread_;
#else
	pthread_t thread_;
#endif
	bool is_running_;
};

extern "C"
{
#ifdef _MSC_VER
	unsigned int _thread_proc(void* param);
#else
	void* _thread_proc(void* param);
#endif
}

}

#endif
