/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: test_log4cplus.h
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#ifndef __VBASE_TEST_LOG4CPLUS_H__
#define __VBASE_TEST_LOG4CPLUS_H__

#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <log4cplus/logger.h>
#include <log4cplus/consoleappender.h>
#include <log4cplus/layout.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/fileappender.h>
using namespace std;
using namespace log4cplus;
using namespace log4cplus::helpers;

class test_log4cplus
{
public:
    void test_console_appender()
    {
        /* step 1: Instantiate an appender object */
        SharedObjectPtr<Appender> _append(new ConsoleAppender());
        _append->setName(LOG4CPLUS_TEXT("ConsoleAppender"));
        /* step 2: Instantiate a layout object */
        //std::string pattern = ;   
        std::auto_ptr<Layout> _layout(new PatternLayout(LOG4CPLUS_TEXT("%D{%m/%d/%y %H:%M:%S}  - %m [%l]%n")));
        /* step 3: Attach the layout object to the appender */
        _append->setLayout(_layout);
        /* step 4: Instantiate a logger object */
        Logger _logger = Logger::getInstance(LOG4CPLUS_TEXT("test"));
        /* step 5: Attach the appender object to the logger  */
        _logger.addAppender(_append);
        /* step 6: Set a priority for the logger  */
        _logger.setLogLevel(WARN_LOG_LEVEL);
        /* log activity */
        LOG4CPLUS_DEBUG(_logger, "This is the FIRST log message");
        LOG4CPLUS_WARN(_logger, "This is the SECOND log message");
        LOG4CPLUS_TRACE(_logger, "This is" << " just a t" << "est." << std::endl);
        LOG4CPLUS_DEBUG(_logger, "This is a bool: " << true);
        LOG4CPLUS_INFO(_logger, "This is a char: " << 'x');
        LOG4CPLUS_WARN(_logger, "This is a int: " << 1000);
        LOG4CPLUS_ERROR(_logger, "This is a long(hex): " << std::hex << 100000000);
        LOG4CPLUS_FATAL(_logger, "This is a double: " << std::setprecision(15) << 1.2345234234);
    }

    //简洁使用模式
    void test_file_appender()
    {
        /* step 1: Instantiate an appender object */
        SharedObjectPtr<Appender> _append(new FileAppender(LOG4CPLUS_TEXT("test_file_appender.log")));
        _append->setName(LOG4CPLUS_TEXT("FileAppender"));
        /* step 4: Instantiate a logger object */
        Logger _logger = Logger::getInstance(LOG4CPLUS_TEXT("test"));
        /* step 5: Attach the appender object to the logger  */
        _logger.addAppender(_append);
        /* log activity */
        auto begin_clock = chrono::system_clock::now();
        for (int i = 0; i < 500; i++) {
            LOG4CPLUS_WARN(_logger, "Entering loop #" << i << "End line#");
            LOG4CPLUS_ERROR(_logger, "Entering loop #" << i << "End line#");
        }
        cout << chrono::duration_cast<chrono::milliseconds>
            (chrono::system_clock::now() - begin_clock).count() << endl;
    }

};

#endif