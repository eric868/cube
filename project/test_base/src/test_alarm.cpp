/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: test_alarm.cpp
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#include "test_alarm.h"
#include <stdio.h>
#include <base/util.h>
#include <string>
test_alarm::test_alarm()
: m_alarm(NULL)
{

}

test_alarm::~test_alarm()
{
    if (m_alarm) {
        m_alarm->turn_off();
        delete m_alarm;
        m_alarm = NULL;
    }
}

void test_alarm::set_alarm(int hour, int min, int sec)
{
    std::string local_time = base::util::local_time_string();
    printf("alarm begin:%s\n", local_time.c_str());
    m_alarm = new base::alarm(hour, min, sec);
    m_alarm->signal_alarm_.connect(this, &test_alarm::alarm_callback);
    m_alarm->turn_on();
}

void test_alarm::alarm_callback(base::alarm_info& ainfo, struct tm* t)
{
    printf("alarm fired,hour:%d, min:%d, sec:%d\n", 
        t->tm_hour, t->tm_min, t->tm_sec);
}


