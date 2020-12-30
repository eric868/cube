/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: event.cpp
Version: 1.0
Date: 2016.1.13

History:
base     2016.1.13   1.0     Create
******************************************************************************/

#include "event.h"
#include "trace.h"
#ifdef _MSC_VER
#include <crtdbg.h>
#else
#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>
#endif

namespace base {

event::event()
{
#ifdef _MSC_VER
    handle_ = ::CreateEvent(NULL, true, false, NULL);
    if (handle_ == NULL) {
        TRACE_ERROR("base", 0, "create event failed, errno:%d", ::GetLastError());
    }
#else
    event_ = 0;
    pthread_mutex_init(&cs_, NULL);
    pthread_cond_init(&cond_, NULL);
#endif
}
    
event::~event()
{
#ifdef _MSC_VER
    ::CloseHandle(handle_);
#else
    pthread_cond_destroy(&cond_);
    pthread_mutex_destroy(&cs_);
#endif
}
    
#ifdef _MSC_VER
    
event::event(LPSECURITY_ATTRIBUTES attributes, bool manual_reset, bool initial_state)
{
    handle_ = ::CreateEvent(attributes, manual_reset, initial_state, NULL);
    if (handle_ == NULL) {
		TRACE_ERROR("base", 0, "create event failed, errno:%d", ::GetLastError());
    }
}
    
event::event(LPSECURITY_ATTRIBUTES attributes, bool manual_reset, bool initial_state, const std::string& name)
{
    handle_ = ::CreateEvent(attributes, manual_reset, initial_state, (LPCWSTR)(name.c_str()));
    if (handle_ == NULL) {
		TRACE_ERROR("base", 0, "create event failed, errno:%d", ::GetLastError());
    }
}
    
#endif

bool event::wait(long milli_seconds)
{
#ifdef _MSC_VER
    DWORD result = ::WaitForSingleObject(handle_, milli_seconds);
    if (result == WAIT_TIMEOUT) {
		return false;
    }
    else if (result == WAIT_OBJECT_0) {
        return true;
    }
    else {
        TRACE_ERROR("base", 0, "event::wait() - WaitForSingleObject, errno:%d", ::GetLastError());
    }
    return false;
#else
    int ret = 0;
    pthread_mutex_lock(&cs_);
    if (event_ == 0) {
        if (milli_seconds < 0) {
            do {
                ret = pthread_cond_wait(&cond_, &cs_);
            } while (event_ == 0 && ret == 0);
        }
        else {
            struct timespec abs_ts;
            struct timeval cur_tv;
            gettimeofday(&cur_tv, NULL);
            abs_ts.tv_sec = cur_tv.tv_sec + milli_seconds / 1000;
            abs_ts.tv_nsec = cur_tv.tv_usec * 1000 + milli_seconds % 1000 * 1000000;
            if (abs_ts.tv_nsec > 1000000000) {
                abs_ts.tv_nsec -= 1000000000;
                abs_ts.tv_sec += 1;
            }
            do {
                ret = pthread_cond_timedwait(&cond_, &cs_, &abs_ts);
            } while (event_ == 0 && ret == 0);
        }
    }
    else if (event_ > 0) {
        event_--;
    }
    pthread_mutex_unlock(&cs_);
    return (ret == 0);
#endif
}
    
void event::set()
{
#ifdef _MSC_VER
    if (!::SetEvent(handle_)) {
		TRACE_ERROR("base", 0, "event::set(), errno:%d", ::GetLastError());
    }
#else
    pthread_mutex_lock(&cs_);
    event_++;
    pthread_cond_signal(&cond_);
    pthread_mutex_unlock(&cs_);
#endif
}
    
void event::reset()
{
#ifdef _MSC_VER
    if (!::ResetEvent(handle_)) {
		TRACE_ERROR("base", 0, "event::reset(), errno:%d", ::GetLastError());
    }
#else
    pthread_mutex_lock(&cs_);
    event_ = 0;
    pthread_mutex_unlock(&cs_);
#endif
}
    
void event::pulse()
{
#ifdef _MSC_VER
    if (!::PulseEvent(handle_)) {
		TRACE_ERROR("base", 0, "event::pulse(), errno:%d", ::GetLastError());
    }
#endif
}

}
