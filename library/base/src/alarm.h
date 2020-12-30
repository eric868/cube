/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: alarm.h
Version: 1.0
Date: 2016.1.13

History:
base     2016.1.13   1.0     Create
******************************************************************************/

#ifndef __BASE_ALARM_H__
#define __BASE_ALARM_H__

#include <vector>
#include "sigslot.h"
#include "timer.h"

namespace base
{

struct alarm_info
{
	int hour;
	int minute;
	int second;

	alarm_info()
		: hour(0)
		, minute(0)
		, second(0)
	{}
};

class COMMON_API alarm
{
public:
	alarm(int hour = 0, int minute = 0, int second = 0);
	~alarm();

public:
	void turn_on();
	void turn_off();

protected:
	void calc_fire_period();
	void set_fire_time();

	void on_timeout(void* param);
	static void on_timeout(void* obj, void* param);

public:
	sigslot::signal2<alarm_info&, struct tm*> signal_alarm_;

private:
	alarm_info ainfo_;
	long fire_time_;
	long fire_period_;
	timer timer_;
};

}

#endif
