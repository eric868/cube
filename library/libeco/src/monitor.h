#ifndef MONITOR_H_
#define MONITOR_H_

#include <mutex>
#include <condition_variable>
#include <chrono> 
#include "project.h"

namespace eco{;


////////////////////////////////////////////////////////////////////////////////
class Monitor : public Object<Monitor>
{
public:
    /*@ init coordinator with tasks total number, when all tasks have been
    finished, coordinator will notify waiter ok, else fail.
    */
    inline explicit Monitor(IN int32_t task_count = 1)
        : m_task_count(task_count)
    {}

    /*@ reset monitor to wait.*/
    inline void reset(IN int32_t task_count = 1)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_task_count = task_count;
    }

    /*@ coordinator wait tasks finish.
    * @ return: tasks finised return true, else return false.
    */
    inline bool wait()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (m_task_count > 0)
        {
            m_cond_var.wait(lock);
        }
        return (m_task_count == 0);
    }

    /*@ coordinator timed wait tasks finish.
    * @ return: if tasks finised: ok; if tasks fail: return fail; else return
    timeout.
    */
    inline eco::Result timed_wait(IN int32_t millsec)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (m_task_count > 0)
        {
            if ( m_cond_var.wait_for(lock, std::chrono::milliseconds(millsec)) 
                == std::cv_status::timeout)
            {
                return eco::timeout;
            }
        }
        return (m_task_count == 0) ? eco::ok : eco::fail;
    }

    // finish one task.
    inline void finish()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_task_count = 0;
        m_cond_var.notify_all();
    }

    // finish one task.
    inline void finish_one()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        --m_task_count;
        if (m_task_count == 0)
        {
            m_cond_var.notify_all();
        }
    }

    // one task failed.
    inline void fail()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_task_count = -1;
        m_cond_var.notify_all();
    }

private:
    int32_t m_task_count;
    std::mutex m_mutex;
    std::condition_variable m_cond_var;
};


////////////////////////////////////////////////////////////////////////////////
}// ns::eco
#endif