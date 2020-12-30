/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: event.cpp
Version: 1.0
Date: 2016.1.13

History:
eric        2018.4.9   1.0     Create
******************************************************************************/

#include "event.h"
#include "iostream"
namespace eco {

#ifdef _MSC_VER
Event::Event()
{
    handle_ = ::CreateEvent(NULL, true, false, NULL);
    if (handle_ == NULL) {
        std::cout << "create event failed, errno:" << ::GetLastError() << std::endl;
    }
}

Event::~Event()
{
    ::CloseHandle(handle_);
}

void Event::SignalEvent()
{
    if (!::SetEvent(handle_)) {
        std::cout << "event::set(), errno:" << ::GetLastError() << std::endl;
    }
}

void Event::WaitEvent()
{
    if (!::ResetEvent(handle_)) {
        std::cout << "event::reset(), errno:%d" << ::GetLastError() << std::endl;
    }
    DWORD result = ::WaitForSingleObject(handle_, INFINITE);
    if (result == WAIT_OBJECT_0) {
        return;
    }
    else {
        std::cout << "event::wait() - WaitForSingleObject, errno:%d" << ::GetLastError() << std::endl;
    }
}

#else
Event::Event()
{
    bIsSignal = false;
    pthread_cond_init(&cond, NULL);
    pthread_mutex_init(&mutex, NULL);
}

Event::~Event()
{
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);
}


void Event::SignalEvent()
{
    pthread_mutex_lock(&mutex);
    bIsSignal = true;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
}

void Event::WaitEvent()
{
    pthread_mutex_lock(&mutex);
    bIsSignal = false;
    while (!bIsSignal) {
        pthread_cond_wait(&cond, &mutex);
    }
    pthread_mutex_unlock(&mutex);

}

#endif

}
