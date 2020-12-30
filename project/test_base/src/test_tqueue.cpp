/*****************************************************************************
VBase Copyright (c) 2015. All Rights Reserved.

FileName: test_tqueue.cpp
Version: 1.0
Date: 2015.9.16

History:
ericsheng     2015.9.15   1.0     Create
******************************************************************************/


#include "test_tqueue.h"
#include <sys/types.h>
#include <iostream>
#include <stdio.h>
#include <time.h>

#define COUNT_NUM 100

#define gettid() syscall(__NR_gettid)

test_tqueue::test_tqueue()
{
    tq_ = new base::tqueue<int>();
    tqstr_ = new base::tqueue<char*>();
}

test_tqueue::~test_tqueue()
{
}

void test_tqueue::mult_thread_test()
{
	base::thread t1;
	for (int i = 0;  i < 1000; ++ i) {
		flagnum = i;
        t1.create((base::thread::thread_proc_t)test_tqueue::thread_cb1, this);
        t1.create((base::thread::thread_proc_t)test_tqueue::thread_cb2, this);
	}

	t1.join();
}

void test_tqueue::single_thread_test()
{
	int count = 0;

		char* msg =  new char[4096] ;
		memset(msg, 'a', 4096 - 1);
		msg[4096 - 1] = '\0';
		int max = 10;
		clock_t start, mid, end;
		cout << "msg: " << msg << " max: "<<max <<endl;
		start = clock();
		for (int i = 0; i < max; ++i) {
			if(tqstr_->post_msg_mul(msg))
				count++;
			else
				cout<<"failed post tqstr_~~~"<<endl;
		}

		mid = clock();

		char* outmsg;
		int getcount = 0;
		int errcount = 0;
		while(tqstr_->get_msg_mul(outmsg))
		{
			if(strcmp(msg, outmsg) != 0) {
				cout<<"error: msg: "<<msg << " outmsg: " << outmsg<<endl;
				errcount++;
			}

			getcount++;
		}

		end = clock();
		cout<<"send count: "<< count << " send time: " << (double)(mid - start)/CLOCKS_PER_SEC <<endl;
		cout<<"get count : "<< getcount <<" get time: "<< (double)(end - mid)/CLOCKS_PER_SEC<<endl;
		cout<<"err count: "<< errcount<<endl;
}
void test_tqueue::thread_cb1(void* arg)
{
	test_tqueue* tt = (test_tqueue*)arg;
    base::tqueue<char*>* tq = tt->tqstr_;
	int count = 0;
	while(true)
    {	
		char* msg =  new char[4096] ;
		memset(msg, 'c', sizeof(msg) - 1);
		msg[sizeof(msg) - 1] = '\0';
		if(tq->post_msg_mul(msg))
			cout<<"succeed post id: "<<tt->flagnum<<" tqstr_ count: "<< count++ <<endl;
		else
			cout<<"failed post tqstr_~~~" <<endl;
        base::thread::sleep(100);
	}
}

void test_tqueue::thread_cb2(void* arg)
{
	test_tqueue* tt = (test_tqueue*)arg;
    base::tqueue<char*>* tq = tt->tqstr_;
	int count = 0;
	char* msg;
//	memset(msg, 0, sizeof(msg));
	while(true)
	{
		if(tq->get_msg_mul(msg))
			cout<<"succeed get id: "<<tt->flagnum<< " count:tqstr_" << count++ << " msg : " << msg <<endl;
		else
			cout<<"failed get tqstr_~~~ " <<endl;
        base::thread::sleep(100);
	}
}

