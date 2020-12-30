/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: state.h
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#ifndef __UTILITY_STATE_H__
#define __UTILITY_STATE_H__

#include "utility.h"
#include <atomic>

namespace utility { 

    class state
    {
    public:
        // object state flag.
        enum
        {
            state_no = 0x00,
            state_ok = 0x01,

            state_a = 0x02,
            state_b = 0x04,
            state_c = 0x08,
            state_d = 0x10,
            state_e = 0x20,
            state_f = 0x40,
            state_g = 0x80,
        };

    public:
        state() : m_value(state_no)
        {}

        inline void none()
        {
            m_value = state_no;
        }
        inline void ok()
        {
            add(state_ok);
        }

        inline bool isok() const
        {
            return has(state_ok);
        }

        inline bool isnone() const
        {
            return (m_value == state_no);
        }

    public:
        // add state.
        inline void add(IN const uint8_t state_val)
        {
            m_value |= state_val;
        }

        // remove state.
        inline void del(const uint8_t state_val)
        {
            m_value &= ~state_val;
        }

        // whether has dedicated state.
        inline bool has(const uint8_t state_val) const
        {
            return (m_value & state_val) > 0;
        }

        inline bool operator==(const state& state) const
        {
            return m_value == state.m_value;
        }

    private:
        // object state value.
        std::atomic<uint8_t> m_value;
    };

}// utility
#endif
