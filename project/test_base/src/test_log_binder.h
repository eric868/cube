/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: test_log_binder.h
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#ifndef BASE_TEST_LOG_H_
#define BASE_TEST_LOG_H_

#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <log4cplus/logger.h>
#include <log4cplus/consoleappender.h>
#include <log4cplus/layout.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/fileappender.h>
#include "base/trace.h"
#include "base/log_binder.h"
using namespace std;
using namespace log4cplus;
using namespace log4cplus::helpers;

class test_log_binder
{
public:
    test_log_binder(){};
    ~test_log_binder(){}; 

public:
    void test()
    {
        if (!base::default_log_binder::bind_trace("./config/log_config.xml")) {
            std::cout << "log bind_trace failed" << std::endl;
            return;
        }
        base::trace::enable_std_output(false);
        base::trace::set_trace_level_filter(base::TRACE_LEVEL_SYSTEM);

        auto begin_clock = chrono::system_clock::now();
        for (int i = 0; i < 100; i++) {
            TRACE_ERROR("test_log_binder", 10000, "%s", "this is log4cplus packet");
            TRACE_SYSTEM("test_log_binder", "test_log_binder (%s:%d)", "hello", i);
        }
        auto end_clock = chrono::system_clock::now();
        auto diff = end_clock - begin_clock;
        cout << chrono::duration_cast<chrono::milliseconds>(diff).count() << endl;
    }
};

#endif
