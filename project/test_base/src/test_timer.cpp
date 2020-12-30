/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: test_timer.h
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#include "test_timer.h"
#ifndef WIN32
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <stdio.h>
#endif

#include <stdlib.h>

#define gettid() syscall(__NR_gettid)

test_timer::test_timer()
	: create_flag(false)
{
}

test_timer::~test_timer()
{
}

void test_timer::test_settimer_single()
{
    base::timer t1, t2;
	t1.set_timer(this, test_timer::timer_cb, NULL, 1000, true);
	printf("timer begin~~\n");
	//t1.fire();
	t1.start();

    t2.set_timer(this, test_timer::timer_cb2, NULL, 2000, true);
    printf("timer begin~~\n");
    //t2.fire();
    t2.start();
	while(getchar() != '0') {};
	printf("timer end~~~");
	t1.reset();
    t2.reset();
}

void test_timer::test_settimer_multi()
{
	start();
}

void test_timer::timer_cb(void* obj, void* param)
{
	printf("this is 1 test~~~~\n");
//	printf("processid:%lu threadid : %lu\n", getpid(),gettid());
}

void test_timer::timer_cb2(void* obj, void* param)
{
	printf("this is 2 test~~~~\n");
//	printf("processid:%lu threadid : %lu\n", getpid(),gettid());
}

void test_timer::timer_cb3(void* obj, void* param)
{
	int fg = *((int*)param);
	printf("this is %d test~~~~\n", fg);
//	printf("processid:%lu threadid : %lu\n", getpid(),gettid());
}

void test_timer::thread_cb(void* obj)
{
	test_timer* tt = (test_timer*)obj;
	int fg = tt->flagid;
    base::timer t2;
	t2.set_timer(obj, test_timer::timer_cb3, &fg, 1000, true);
	t2.start();
	getchar();
}

void test_timer::run()
{
    base::timer t2;
	t2.set_timer(this, test_timer::timer_cb2, NULL, 1000, true);
	t2.start();
	getchar();
}

void test_timer::start()
{
	process_thread::start();
	create_flag = true;
	thread_group = new base::thread[100];
	for (int var = 0; var < 100; ++var) {
		flagid = var;
        thread_group[var].create((base::thread::thread_proc_t)thread_cb, this);
	}
	getchar();
}

void test_timer::stop()
{

}
