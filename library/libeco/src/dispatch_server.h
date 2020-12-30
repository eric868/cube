/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: dispatch_server.h
Version: 1.0
Date: 2018.5.4

History:
eric        2018.5.4   1.0     Create
******************************************************************************/

#ifndef DISPATCH_SERVER_H_
#define DISPATCH_SERVER_H_

#include <atomic>
#include "dispatch_handler.h"
#include "message_queue.h"
#include "thread_pool.h"

namespace eco {;

template<typename T, typename M>
class DispatchServer
{
public:
    typedef DispatchHandler<T, M> Handler;
    typedef typename Handler::HandlerFunc HandlerFunc;

    DispatchServer(void)
    {}

    ~DispatchServer(void)
    {
        Stop();
    }

    void Stop()
    {
        std::call_once(m_flag, [this] {StopThreadGroup();});
    }

    void SetDispatch(const T& type, HandlerFunc func)
    {
        m_message_handler.set_dispatch(type, func);
    }

    void SetDefault(HandlerFunc func)
    {
        m_message_handler.set_default(func);
    }

    void Post(M&& msg)
    {
        m_message_queue.Put(std::forward<M>(msg));
    }

    void Post(const M& msg)
    {
        m_message_queue.Put(msg);
    }

    void Start(int thread_size = 1)
    {
        m_running = true;

        m_thread_pool.Run(std::bind(&DispatchServer::RunInThread, this),
            thread_size);
    }

private:
    void RunInThread()
    {
        while (m_running) {
            std::deque<M> deque;
            m_message_queue.Take(deque);

            for (auto& val : deque) {
                if (!m_running) return;
                m_message_handler(val);
            }
        }// end while
    }

    void StopThreadGroup()
    {
        m_message_queue.Stop();
        m_running = false;
        m_thread_pool.Join();
    }

    MessageQueue<M> m_message_queue;
    DispatchHandler<T, M> m_message_handler;
    ThreadPool m_thread_pool;
    std::atomic_bool m_running; //是否停止的标志
    std::once_flag m_flag;
};

}
#endif
