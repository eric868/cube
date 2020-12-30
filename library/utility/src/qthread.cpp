/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: qthread.cpp
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#include "qthread.h"
#include "applog.h"
#include "semaphore.h"
#include <iostream>
using namespace std;

#ifdef _MSC_VER
#include <process.h>    
#endif

namespace utility 
{
	thread_runner::thread_runner() 
		:id_(0), state_(THREAD_READY)
    {
#ifdef _MSC_VER
        join_sem_ = new semaphore(0,1);
#endif
    }

	thread_runner::~thread_runner()
    {
#ifdef _MSC_VER
        if(join_sem_) {
            delete join_sem_;
        }
#endif
    }

	void thread_runner::request_terminate()
    {
		if (state_ == THREAD_READY || state_ == THREAD_RUNNING) {
			state_ = THREAD_REQUEST_STOP;
        }
    }

	void thread_runner::join()
    {
		if (state_ != THREAD_RUNNING && state_ != THREAD_REQUEST_STOP) {
            return ;
        }
#ifdef _MSC_VER
		while (!join_sem_->try_wait() && state_ != THREAD_STOP) {
            Sleep(100);
        }
#elif defined __GNUG__
        pthread_join(tid_, 0);
#endif
    }

	void thread_runner::before_run()
    {
		if (state_ == THREAD_READY) {
			state_ = THREAD_RUNNING;
        }
    }

	void thread_runner::after_terminate()
    {
		state_ = THREAD_STOP;

#ifdef _MSC_VER
        join_sem_->post(1);
#elif defined __GNUG__
        // pthread_detach(pthread_self());
#endif
		thread_manager::remove_thread(*this);
    }

	map<unsigned long, thread_runner*> thread_manager::thread_map_;
	mutex* thread_manager::mutex_ = new mutex();

	unsigned int thread_manager::create_thread(thread_runner & t_obj)
    {
        int ret = 0;
        scope_mutex locker(mutex_);
        t_obj.id_ = (unsigned long)&t_obj;
        thread_map_[t_obj.id_] = &t_obj;
#ifdef _MSC_VER
		ret = _beginthread(thread_manager::thread_func, 1000, &t_obj);
        if(ret < 0) {
            APP_LOG(applog::LOG_CRITICAL)<<"Create thread failed";
        }
#elif defined __GNUG__
		ret = pthread_create(&t_obj.tid_, 0, thread_manager::thread_func, &t_obj);
        if(ret) {
            APP_LOG(applog::LOG_CRITICAL)<<"Create thread failed, error code:" 
				<< ret;
        }
#endif 
        return ret;
    }

	void thread_manager::remove_thread(thread_runner & t_obj)
    {
        unsigned long thread_id = t_obj.get_id_();
        if(thread_id == 0) { return; }
        scope_mutex locker(mutex_);
		map<unsigned long, thread_runner*>::iterator it 
			= thread_map_.find(thread_id);
        if(it != thread_map_.end()) {
			thread_runner* t = thread_map_[thread_id];
            thread_map_.erase(thread_id);
			if (t->state_ == thread_runner::THREAD_RUNNING) {
                t->request_terminate();
                t->join();
            }
        } else {
            APP_LOG(applog::LOG_WARNING) << 
				"not found thread id : " << (unsigned int)thread_id ;
        }
    }

#ifdef WIN32
	void thread_manager::thread_func(void* arg)
#else
    void* QCThreadManager::thread_func(void* arg)
#endif
    {
		thread_runner* runner = (thread_runner*)arg;
        runner->before_run();
        runner->run();
        runner->after_terminate();
#ifdef WIN32
        return ;
#else
        return 0;
#endif
    }
}
