/*****************************************************************************
 VBase Copyright (c) 2015. All Rights Reserved.

 FileName: test_dispatch.h
 Version: 1.0
 Date: 2015.9.15
 
 History:
 ericsheng     2015.9.15   1.0     Create
 ******************************************************************************/

#ifndef BASE_TEST_DISPATCH_H_
#define BASE_TEST_DISPATCH_H_

#include "base/dispatch.h"

class test_dispatch
{
public:
	test_dispatch();
	~test_dispatch();

public:
	void test_timer();
	void test_async_dispatch();
	void test_mul_async_dispatch();

public:
	static void dispatch_callback(void* obj, void* param);
};

#endif
