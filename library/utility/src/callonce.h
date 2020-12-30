/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: callonce.h
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#ifndef __UTILITY_CALLONCE_H__
#define __UTILITY_CALLONCE_H__

#include "mutex.h"
#include <functional>
#include "state.h"

namespace utility {

class COMMON_API callonce
{
public:
    typedef std::function<void(void)> func_t;

    inline callonce()
    {}

    inline void reset()
    {
        m_finish.none();
    }

    inline bool Finished() const
    {
        return m_finish.isok();
    }

    /*@ call once work funciton.*/
    inline void Work(IN func_t func)
    {
        if (!m_finish.isok())	// double check.
        {
            scope_mutex lock(&m_finish_mutex);
            if (!m_finish.isok())
            {
                func();
                m_finish.ok();
            }
        }// end if.
    }

private:
    state m_finish;
	mutex m_finish_mutex;
};

}
#endif