/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: test_strand.h
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#ifndef __TEST_DEADLINE_TIMER_H__
#define __TEST_DEADLINE_TIMER_H__

#include "test_common.h"

inline void deadline_timer_print(const boost::system::error_code&)
{
    cout << "Hello, world!" << endl;
}

class test_deadline_timer
{
public:
    test_deadline_timer()
        :m_deadline_timer(m_service, boost::posix_time::seconds(5))
    {
    }
    ~test_deadline_timer(){}



    void handle_wait(const boost::system::error_code& error,
        boost::asio::deadline_timer& t,
        int& count)
    {
        if (!error)
        {
            cout << count << endl;
            if (count++ < 5)
            {
                t.expires_from_now(boost::posix_time::seconds(3));
                t.async_wait(boost::bind(&test_deadline_timer::handle_wait, this,
                    boost::asio::placeholders::error,
                    boost::ref(t),
                    boost::ref(count)));
                if (count == 3)
                {
                    t.cancel();
                }

            }
        }
    }

    // 同步方法  
    void test_timer_syn()
    {
        //m_deadline_timer.expires_from_now(boost::posix_time::seconds(5));
        Sleep(5000);
        m_deadline_timer.wait();
        cout << "Hello syn deadline_timer!" << endl;
    }

    // 异步方法: 3秒后执行print方法.   
    void test_timer_asyn()
    {
        m_deadline_timer.expires_from_now(boost::posix_time::seconds(2));
        m_deadline_timer.async_wait(deadline_timer_print);
        cout << "After async_wait..." << endl;
        m_service.run();
    }

    // 异步循环执行方法:   
    void test_timer_asyn_loop()
    {
        size_t a = m_deadline_timer.expires_from_now(boost::posix_time::seconds(1));
        int count = 0;
        m_deadline_timer.async_wait(boost::bind(&test_deadline_timer::handle_wait, this,
            boost::asio::placeholders::error,
            boost::ref(m_deadline_timer),
            boost::ref(count)));
        m_service.run();
    }


private:
    boost::asio::io_service m_service;
    boost::asio::deadline_timer m_deadline_timer;
};

#endif
