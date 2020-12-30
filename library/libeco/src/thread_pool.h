/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: thread_pool.h
Version: 1.0
Date: 2016.1.13

History:
eric     2016.4.13   1.0     Create
******************************************************************************/
#ifndef THREAD_POOL_H_
#define THREAD_POOL_H_

#include <list>
#include <thread>
#include <iostream>
#include <functional>
#include <memory>

namespace eco{;

class ThreadPool
{
public:
    void Run(std::function<void(void)> func, uint32_t thread_size = 1)
    {
        for (uint32_t i = 0; i < thread_size; ++i) {
            m_threads.push_back(std::make_shared<std::thread>(func));
        }
    }

    void Join()
    {
        for (auto val : m_threads) {
            if (val) {
                val->join();
            }
        }
        m_threads.clear();
    }

private:
    std::list<std::shared_ptr<std::thread>> m_threads;
};

}

#endif
