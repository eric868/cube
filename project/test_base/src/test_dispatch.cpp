/*****************************************************************************
 VBase Copyright (c) 2015. All Rights Reserved.

 FileName: test_dictionary.h
 Version: 1.0
 Date: 2015.8.21

 History:
 ericsheng     2015.8.21   1.0     Create
 ******************************************************************************/

#include "test_dispatch.h"

test_dispatch::test_dispatch()
{

}

test_dispatch::~test_dispatch()
{

}

void test_dispatch::test_timer()
{
    test_async_dispatch();
	getchar();
}

void test_dispatch::test_async_dispatch()
{
    base::dispatch(this, test_dispatch::dispatch_callback, NULL);
    printf("test_async_dispatch\n");
}

void test_dispatch::test_mul_async_dispatch()
{
	for (int i = 0; i < 100; i++) {
		base::dispatch(this, test_dispatch::dispatch_callback, NULL);
        //printf("test_mul_async_dispatch\n");
	}
}

void test_dispatch::dispatch_callback(void* obj, void* param)
{
	static int count = 1;
	printf("async dispatch fired, %d\n", count++);
}

