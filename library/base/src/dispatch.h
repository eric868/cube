/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: dispatch.h
Version: 1.0
Date: 2016.1.13

History:
base     2016.1.13   1.0     Create
******************************************************************************/

#ifndef __VBASE_DISPATCH_H__
#define __VBASE_DISPATCH_H__

#include "tqueue.h"
#include "thread.h"
#include "event.h"
#include "timer.h"

namespace base
{

typedef timer_callback dispatch_callback;

/*
 * dispatch operation to the process thread
 * @param obj the obj that will be passed back to the function
 * @param cb target callback function
 * @param param the param that will be passed back to the function
 * @param delay the delay time before calling the function, in milliseconds.
 */
void COMMON_API dispatch(void* obj, dispatch_callback cb, void* param);


/****************************************************************************/

struct dispatch_op
{
	void* obj;
	dispatch_callback cb;
	void* param;

	dispatch_op()
		: obj(NULL)
		, cb(NULL)
		, param(NULL)
	{}
};

class dispatch_scheduler
	: public base::process_thread
{
protected:
	dispatch_scheduler();
	virtual ~dispatch_scheduler();

public:
	static dispatch_scheduler* shared_instance();
	static void release_instance();

public:
	void dispatch(void* obj, dispatch_callback cb, void* param);

public:
	virtual void run();
	virtual void start();
	virtual void stop();

public:
	void attach_timer(timer* t);
	void detach_timer(timer* t);
    void fire_timer(timer* t);

protected:
	int check_timers();

private:
#if defined(PLATFORM_WINDOWS)
	static DWORD WINAPI message_process_thread(void* param);
#else
	static void* message_process_thread(void* param);
#endif
	void process_message();

private:
	thread* process_threads_;
	bool stop_process_threads_;
	tqueue<dispatch_op>* op_queue_;
	event* op_event_;

	mutex mutex_;
	std::vector<timer*> timers_;
	event* schedule_event_;

	static dispatch_scheduler* instance_;
	static int dispatch_thread_count_;
	static mutex create_mutex_;
};

}

#endif
