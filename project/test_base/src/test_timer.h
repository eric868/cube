/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: test_timer.h
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#ifndef __VBASE_TEST_TIMER_H__
#define __VBASE_TEST_TIMER_H__

#include "base/timer.h"
#include "base/thread.h"


class test_timer : public base::process_thread
{
public:
	test_timer();
	~test_timer();

public:
	void test_settimer_single();
	void test_settimer_multi();
	virtual void run();

	virtual void start();
	virtual void stop();

public:
	static void timer_cb(void* obj, void* param);
	static void timer_cb2(void* obj, void* param);
	static void timer_cb3(void* obj, void* param);
	static void thread_cb(void* obj);

private:
	bool create_flag;
	base::thread* thread_group;
	int flagid;
};

#endif
