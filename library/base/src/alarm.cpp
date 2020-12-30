/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: alarm.cpp
Version: 1.0
Date: 2016.1.13

History:
base     2016.1.13   1.0     Create
******************************************************************************/

#include "alarm.h"
#include "util.h"
#include "dispatch.h"
#include <time.h>

namespace base
{

alarm::alarm(int hour, int minute, int second)
	: fire_time_(0)
	, fire_period_(0)
{
	ainfo_.hour = hour;
	ainfo_.minute = minute;
	ainfo_.second = second;

	calc_fire_period();
}

alarm::~alarm()
{
	turn_off();
}

void alarm::turn_on()
{
	set_fire_time();
	timer_.set_timer(this, on_timeout, NULL, 1000, true);
	timer_.start();
}

void alarm::turn_off()
{
	timer_.reset();
	fire_time_ = 0;
}

void alarm::calc_fire_period()
{
	if (ainfo_.minute < 0) {
		fire_period_ = 60;
	}
	else if (ainfo_.hour < 0) {
		fire_period_ = 60 * 60;
	}
	else {
		fire_period_ = 60 * 60 * 24;
	}
}

void alarm::set_fire_time()
{
	/* first fire time */
	if (fire_time_ == 0)
	{
		time_t t = time(NULL);
		struct tm* dt = localtime(&t);
		int param = 0;
		if (ainfo_.minute < 0) {
			if (ainfo_.second <= dt->tm_sec) {
				param = 1;
			}
			fire_time_ = param * 60 + (ainfo_.second - dt->tm_sec);
		}
		else if (ainfo_.hour < 0) {
			if (ainfo_.minute < dt->tm_min || (ainfo_.minute == dt->tm_min && ainfo_.second <= dt->tm_sec)) {
				param = 1;
			}
			fire_time_ = 60 * (param * 60 + (ainfo_.minute - dt->tm_min)) + (ainfo_.second - dt->tm_sec);
		}
		else {
			if (ainfo_.hour < dt->tm_hour || (ainfo_.hour == dt->tm_hour && ainfo_.minute < dt->tm_min) ||
				(ainfo_.hour == dt->tm_hour && ainfo_.minute == dt->tm_min && ainfo_.second <= dt->tm_sec)) {
				param = 1;
			}
			fire_time_ = 3600 * (24 * param + (ainfo_.hour - dt->tm_hour)) +
					60 * (ainfo_.minute - dt->tm_min) + (ainfo_.second - dt->tm_sec);
		}
		fire_time_ += t;
	}
	else {
		fire_time_ += fire_period_;
	}
}

void alarm::on_timeout(void* param)
{
	time_t t = time(NULL);

	if (t >= fire_time_) {
		set_fire_time();
		struct tm* dt = localtime(&t);
		signal_alarm_(ainfo_, dt);
	}
}

void alarm::on_timeout(void* obj, void* param)
{
	alarm* m = (alarm*)obj;
	assert(m != NULL);

	m->on_timeout(param);
}

}
