/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: event.h
Version: 1.0
Date: 2016.1.13

History:
base     2016.1.13   1.0     Create
******************************************************************************/

#ifndef __BASE_EVENT_H__
#define __BASE_EVENT_H__

#if defined(_MSC_VER)
#include <Windows.h>
#else
#include <pthread.h>
#endif
#include <assert.h>
#include <string>

#include "base.h"

namespace base {

class COMMON_API event
{
public:
    event();
    ~event();
    
#ifdef _MSC_VER
    event(LPSECURITY_ATTRIBUTES attributes, bool manual_reset, bool initial_state);
    event(LPSECURITY_ATTRIBUTES attributes, bool manual_reset, bool initial_state, const std::string& name);
#endif
    
public:
    bool wait(long milli_seconds = -1);
    void set();
    void reset();
    void pulse();
    
private:
    event(const event& x);
    event& operator=(const event& x);
    
private:
#ifdef _MSC_VER
    HANDLE handle_;
#else
    int event_;
    pthread_mutex_t cs_;
    pthread_cond_t cond_;
#endif
};
    
} // end of namespace base

#endif
