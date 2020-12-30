/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: test_base.h
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#ifndef TEST_BASE_H_
#define TEST_BASE_H_

#include "test_common.h"

class TestBase
{
public:
    TestBase(){}
    ~TestBase(){}

    void Test()
    {
        SetHeartTime(2);
        boost::this_thread::sleep(system_time_);
        cout << __FUNCTION__ << endl;
    }

private:
    void SetHeartTime(int seconds)
    {
        system_time_ = boost::get_system_time() + 
            boost::posix_time::seconds(seconds);
    }
private:
    boost::system_time system_time_;
};

#endif
