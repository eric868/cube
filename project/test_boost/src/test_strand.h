/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: test_strand.h
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#ifndef __TEST_STRAND_H__
#define __TEST_STRAND_H__

#include "test_common.h"

boost::mutex m_mutex;
inline void print(int id)
{
    //boost::mutex::scoped_lock lock(m_mutex);  
    static int count = 0;
    cout << "id: " << id << endl;
    cout << "count: " << ++count << endl;
}


class test_strand
{
public:
    test_strand()
        : m_strand(m_service)
    {
        m_service.stop();
    }
    ~test_strand(){}

    void test_strands()
    {
        boost::thread ios1(&test_strand::ioRun1, this);
        boost::thread ios2(&test_strand::ioRun2, this);

        boost::thread t1(&test_strand::strand_print1, this);
        boost::thread t2(&test_strand::strand_print2, this);
        boost::thread t3(&test_strand::strand_print3, this);
        boost::thread t4(&test_strand::strand_print4, this);

        t1.join();
        t2.join();
        t3.join();
        t4.join();

        m_service.run();
    }

    void test_service()
    {
        boost::thread ios1(&test_strand::ioRun1, this);
        boost::thread ios2(&test_strand::ioRun2, this);

        boost::thread t1(&test_strand::service_print1, this);
        boost::thread t2(&test_strand::service_print2, this);
        boost::thread t3(&test_strand::service_print3, this);
        boost::thread t4(&test_strand::service_print4, this);

        t1.join();
        t2.join();
        t3.join();
        t4.join();

        m_service.run();
    }

private:
    void ioRun1()
    {
        while (true)
        {
            m_service.run();
        }
    }

    void ioRun2()
    {
        while (true)
        {
            m_service.run();
        }
    }

    void strand_print1()
    {
        m_strand.dispatch(boost::bind(print, 1));
    }

    void strand_print2()
    { 
        m_strand.post(boost::bind(print, 2));
    }

    void strand_print3()
    {
        m_strand.post(boost::bind(print, 3));
    }

    void strand_print4()
    {
        m_strand.post(boost::bind(print, 4));
    }

    // 将上面的m_strand换成m_service后，  
    void service_print1()
    {
        m_service.dispatch(boost::bind(print, 1));
    }

    void service_print2()
    {
        m_service.post(boost::bind(print, 2));
    }

    void service_print3()
    {
        m_service.post(boost::bind(print, 3));
    }

    void service_print4()
    {
        m_service.post(boost::bind(print, 4));
    }

private:
    // strand提供串行执行, 能够保证线程安全, 同时被post或dispatch的方法, 不会被并发的执行.   
    // io_service不能保证线程安全  
    boost::asio::io_service m_service;
    boost::asio::strand m_strand;
};

#endif
