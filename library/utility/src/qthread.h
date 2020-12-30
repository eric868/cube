/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: qthread.h
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#ifndef __UTILITY_QTHREAD_H__
#define __UTILITY_QTHREAD_H__

#include <map>
#include "mutex.h"
#include "singleton.h"

#ifdef __GNUG__
#include <pthread.h>
#endif

using namespace std;

namespace utility 
{
    class semaphore;
    class COMMON_API thread_runner
    {
    public:
        enum state{
            THREAD_READY,
            THREAD_RUNNING,
            THREAD_REQUEST_STOP,
            THREAD_STOP,
        };
		thread_runner();
		virtual ~thread_runner();

		friend class thread_manager;

        void join();
        void request_terminate();
        int get_state(){return state_;}
		bool is_alive(){ return (state_ != THREAD_STOP); }
        inline const unsigned long get_id_() const { return id_; }
    protected:
        virtual void before_run();
        virtual void after_terminate();
        virtual void run() = 0;

        unsigned long id_;
        volatile state state_;

    private:
#ifdef WIN32
		semaphore* join_sem_;     
#else
		pthread_t tid_;
#endif
    };

    class COMMON_API thread_manager
	{
        SINGLETON_ONCE(thread_manager);

    public:
		static unsigned int create_thread(thread_runner & t_obj);
		static void remove_thread(thread_runner & t_obj);

    private:
		static map<unsigned long, thread_runner*> thread_map_;
        static mutex* mutex_;
#ifdef WIN32
        static void thread_func(void* arg);
#else
        static void* thread_func(void* arg);
#endif
    };
    SINGLETON_GET(thread_manager);
}
#endif
