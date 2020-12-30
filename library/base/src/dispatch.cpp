/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: dispatch.cpp
Version: 1.0
Date: 2016.1.13

History:
base     2016.1.13   1.0     Create
******************************************************************************/

#include "dispatch.h"
#include "timer.h"
#include "util.h"

namespace base
{

void dispatch(void* obj, dispatch_callback cb, void* param)
{
	dispatch_scheduler::shared_instance()->dispatch(obj, cb, param);
}

dispatch_scheduler* dispatch_scheduler::instance_ = NULL;
int dispatch_scheduler::dispatch_thread_count_ = 2;
mutex dispatch_scheduler::create_mutex_;

dispatch_scheduler::dispatch_scheduler()
	: process_threads_(NULL)
	, stop_process_threads_(false)
	, op_queue_(NULL)
	, op_event_(NULL)
	, schedule_event_(NULL)
{

}

dispatch_scheduler::~dispatch_scheduler()
{

}

dispatch_scheduler* dispatch_scheduler::shared_instance()
{
	if (instance_ == NULL) {
		mutex_scope ms(&create_mutex_);
		if (instance_ == NULL) {
			instance_ = new dispatch_scheduler();
			instance_->start();
		}
	}
	return instance_;
}

void dispatch_scheduler::release_instance()
{
	if (instance_ != NULL) {
		mutex_scope ms(&create_mutex_);
		if (instance_ != NULL) {
			instance_->stop();
			delete instance_;
			instance_ = NULL;
		}
	}
}

void dispatch_scheduler::dispatch(void* obj, dispatch_callback cb, void* param)
{
	assert(cb != NULL);

    dispatch_op op;
    op.cb = cb;
    op.obj = obj;
    op.param = param;
    op_queue_->post_msg_mul(op);
    op_event_->set();
}


void dispatch_scheduler::run()
{
	while (is_running_)
	{
		int ret = check_timers();
		if (ret == 0) {
			schedule_event_->reset();
			schedule_event_->wait(20);
		}
		else {
			thread::sleep(1);
		}
	}
}

void dispatch_scheduler::start()
{
	schedule_event_ = new event();
	process_thread::start();

	op_queue_ = new tqueue<dispatch_op>(8);
	op_event_ = new event();

	stop_process_threads_ = false;
	process_threads_ = new thread[dispatch_thread_count_];
	for (int i = 0; i < dispatch_thread_count_; i++) {
		process_threads_[i].create((thread::thread_proc_t)message_process_thread, this);
	}
}

void dispatch_scheduler::stop()
{
	process_thread::stop();

	if (schedule_event_ != NULL) {
		delete schedule_event_;
		schedule_event_ = NULL;
	}

	stop_process_threads_ = true;
	if (process_threads_ != NULL) {
		for (int i = 0; i < dispatch_thread_count_; i++) {
			process_threads_[i].join();
		}
		delete[]process_threads_;
		process_threads_ = NULL;
	}

	if (op_queue_ != NULL) {
		delete op_queue_;
		op_queue_ = NULL;
	}

	if (op_event_ != NULL) {
		delete op_event_;
		op_event_ = NULL;
	}
}

void dispatch_scheduler::attach_timer(timer* t)
{
	mutex_scope ms(&mutex_);

	t->next_fire_time_ = util::clock() + t->timeout_;
	timers_.push_back(t);

	schedule_event_->set();
}

void dispatch_scheduler::detach_timer(timer* t)
{
	mutex_scope ms(&mutex_);

	for (int i = 0; i < (int)timers_.size(); i++) {
		if (timers_.at(i) == t) {
			timers_.erase(timers_.begin() + i);
			break;
		}
	}
}

void dispatch_scheduler::fire_timer(timer* t)
{
    mutex_scope ms(&mutex_);

    this->dispatch(t->obj_, t->cb_, t->param_);
    t->next_fire_time_ = util::clock() + t->timeout_;
}

int dispatch_scheduler::check_timers()
{
	mutex_scope ms(&mutex_);

	for (int i = 0; i < (int)timers_.size(); ) {
		timer* t = timers_.at(i);
		if (t->next_fire_time_ > 0 && t->next_fire_time_ < util::clock()) {
			this->dispatch(t->obj_, t->cb_, t->param_);
			t->next_fire_time_ = 0;
		}
		if (t->next_fire_time_ == 0) {
			if (t->repeat_) {
				t->next_fire_time_ = util::clock() + t->timeout_;
			}
			else {
				timers_.erase(timers_.begin() + i);
				continue;
			}
		}
		i++;
	}
	return (int)timers_.size();
}

#if defined(PLATFORM_WINDOWS)
DWORD WINAPI dispatch_scheduler::message_process_thread(void* param)
#else
void* dispatch_scheduler::message_process_thread(void* param)
#endif
{
	assert(param != NULL);

	dispatch_scheduler* dscheduler = (dispatch_scheduler*)param;
	dscheduler->process_message();

	return NULL;
}

void dispatch_scheduler::process_message()
{
	dispatch_op dop;
	while (!stop_process_threads_) {
		 if (op_queue_->get_msg_mul(dop)) {
			 dop.cb(dop.obj, dop.param);
		 }
		 else {
			 op_event_->reset();
			 op_event_->wait(20);
		 }
	}
}

}
