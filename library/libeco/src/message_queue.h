/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: message_queue.h
Version: 1.0
Date: 2018.5.4

History:
eric        2018.5.4   1.0     Create
******************************************************************************/

#ifndef MESSAGE_QUEUE_H_
#define MESSAGE_QUEUE_H_

#include <deque>
#include <mutex>
#include <condition_variable>
#include <algorithm>
#include <iostream>
#include <thread>
#include "state.h"

namespace eco {;

template<typename T>
class MessageQueue
{
public:
    enum { default_capacity = 5000 };

    inline MessageQueue(IN const int capacity = default_capacity)
        : m_capacity(capacity)
    {
    }

    inline void Put(IN const T& msg)
    {
        Add(msg);
    }

    inline void Put(IN T&& msg)
    {
        Add(std::forward<T>(msg));
    }

    inline bool Take(OUT std::deque<T>& deque)
    {
        std::unique_lock<std::mutex> locker(m_mutex);
        m_not_empty.wait(locker, [this] {return m_stop || NotEmpty();});
        if (m_stop) {
            return false;
        }

        deque = std::move(m_deque);
        m_not_full.notify_one();
        return true;
    }

    inline bool Take(OUT T& msg)
    {
        std::unique_lock<std::mutex> locker(m_mutex);
        m_not_empty.wait(locker, [this] {return m_stop || NotEmpty();});
        if (m_stop) {
            return false;
        }

        msg = m_deque.front();
        m_deque.pop_front();
        m_not_full.notify_one();
        return true;
    }

    inline void Stop()
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_stop.ok();
        }
        m_not_full.notify_all();
        m_not_empty.notify_all();
    }

    inline bool Empty() const
    {
        std::lock_guard<std::mutex> locker(m_mutex);
        return m_deque.empty();
    }

    inline bool Full() const
    {
        std::lock_guard<std::mutex> locker(m_mutex);
        return m_deque.size() == m_capacity;
    }

    inline int Size() const
    {
        std::lock_guard<std::mutex> locker(m_mutex);
        return m_deque.size();
    }

    ////////////////////////////////////////////////////////////////////////////////
private:
    inline bool NotFull() const
    {
        bool full = m_deque.size() >= m_capacity;
        //        if (full) {
        //            std::cout << "full, waiting, threadid: " << std::this_thread::get_id()
        //                << std::endl;
        //        }
        return !full;
    }

    inline bool NotEmpty() const
    {
        bool empty = m_deque.empty();
        //        if (empty) {
        //            std::cout << "empty, waiting, threadid: " << std::this_thread::get_id()
        //                << std::endl;
        //        }
        return !empty;
    }

    template <typename F>
    inline void Add(IN F&& msg)
    {
        std::unique_lock<std::mutex> locker(m_mutex);
        m_not_full.wait(locker, [this] {return m_stop || NotFull();});
        if (m_stop) {
            return;
        }

        m_deque.push_back(std::forward<F>(msg));
        m_not_empty.notify_one();
    }

private:
    uint32_t m_capacity;
    std::deque<T> m_deque;
    std::mutex m_mutex;
    std::condition_variable m_not_empty;
    std::condition_variable m_not_full;
    eco::atomic::State m_stop;
};

}// ns.eco
#endif
