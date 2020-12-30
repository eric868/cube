#include "alarm.h"
#include <time.h>
#include <iostream>

alarm::alarm(int hour, int minute, int second)
    : fire_time_(0)
    , fire_period_(0)
{
    ainfo_.hour = hour;
    ainfo_.minute = minute;
    ainfo_.second = second;

    calc_fire_period();
    set_fire_time();
}

alarm::~alarm()
{
}

void alarm::set_bc(std::function<void(void)> bc)
{
    bc_ = bc;
}

void alarm::on_timeout()
{
    time_t t = time(NULL);
    if (t >= fire_time_) {
        set_fire_time();
        std::cout << "hello " << std::endl;
    }
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

