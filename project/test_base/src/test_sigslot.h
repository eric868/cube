/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: test_sigslot.h
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#ifndef BASE_TEST_SIGSLOT_H_
#define BASE_TEST_SIGSLOT_H_

#include "base/sigslot.h"
#include "base/util.h"
#include <iostream>

class alarm_clock
{
public:
	sigslot::signal1<int> tick;//闹钟响起
	void send_msg(int num)
	{
		tick(num);
	}
};

struct student : public sigslot::has_slots<>
{
	void weak_up(int num)//对闹钟响起事件的处理 和Tick的参数一一对应
	{
		std::cout << "起床时间 : " <<num<< std::endl;
	}
};

#endif