/******************************************************************************
Copyright (c) 2020. All Rights Reserved.

FileName: timer.h
Version: 1.0
Date: 2018.10.11

History:
eric     2018.10.11   1.0     Create
******************************************************************************/
#ifndef TIMER_H_
#define TIMER_H_

#include "util.h"

namespace eco {

class Timer
{
public:
    Timer()
    {
        start_time_ = util::local_mtimestamp();
    }

    void Restart()
    {
        start_time_ = util::local_mtimestamp();
    }

    int64_t Elapsed() const
    {
        return util::local_mtimestamp() - start_time_;
    }

private:
    int64_t start_time_;
};

}
#endif
