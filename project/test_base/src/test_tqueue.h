/*****************************************************************************
 VBase Copyright (c) 2015. All Rights Reserved.

 FileName: test_tqueue.h
 Version: 1.0
 Date: 2015.9.16

 History:
 ericsheng     2015.9.15   1.0     Create
 ******************************************************************************/

#ifndef BASE_TEST_TQUEUE_H__
#define BASE_TEST_TQUEUE_H__

#include "base/event.h"
#include "base/thread.h"
#include "base/tqueue.h"
#include <string>

using namespace std;

class test_tqueue
{
public:
	test_tqueue();
	~test_tqueue();

public:
	void single_thread_test();
	void mult_thread_test();

public:
	static void thread_cb1(void* arg);
	static void thread_cb2(void* arg);

private:
	base::tqueue<int>* tq_;
	base::tqueue<char*>* tqstr_;
	int flagnum;
};

#endif
