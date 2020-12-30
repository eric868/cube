/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: event.h
Version: 1.0
Date: 2016.1.13

History:
eric        2018.4.9   1.0     Create
******************************************************************************/

#ifndef EVENT_H_
#define EVENT_H_

#ifdef _MSC_VER
#include <WinSock2.h>
#include <Windows.h>
#else
#include <pthread.h>
#include <errno.h>
#endif

namespace eco
{
class Event
{

public:
    Event();
    ~Event();

    void SignalEvent();
    void WaitEvent();

private:

#ifdef _MSC_VER
    HANDLE handle_;
#else
    pthread_cond_t cond;
    pthread_mutex_t mutex;
    bool bIsSignal;
#endif

};
}
#endif
