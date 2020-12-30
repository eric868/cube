/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: timer.cpp
Version: 1.0
Date: 2016.1.13

History:
base     2016.1.13   1.0     Create
******************************************************************************/

#include "timer.h"
#include "util.h"
#include "dispatch.h"

namespace base
{

timer::timer()
	: obj_(NULL)
	, cb_(NULL)
	, param_(NULL)
	, timeout_(0)
	, repeat_(false)
	, next_fire_time_(0)
{

}

timer::~timer()
{
	reset();
}

void timer::set_timer(void* obj, timer_callback cb, void* param, int timeout, bool repeat)
{
	obj_ = obj;
	cb_ = cb;
	param_ = param;
	repeat_ = repeat;

	timeout_ = timeout;
	if (timeout_ <= 0) {
		timeout_ = 1;
	}
	next_fire_time_ = 0;
}

bool timer::start()
{
	reset();

	dispatch_scheduler::shared_instance()->attach_timer(this);
	return true;
}

void timer::fire()
{
	dispatch_scheduler::shared_instance()->fire_timer(this);
}

void timer::reset()
{
	dispatch_scheduler::shared_instance()->detach_timer(this);
}

}
