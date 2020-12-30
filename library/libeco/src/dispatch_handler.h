/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: dispatch_handler.h
Version: 1.0
Date: 2018.5.4

History:
eric        2018.5.4   1.0     Create
******************************************************************************/


#ifndef DISPATCH_HANDLER_H_
#define DISPATCH_HANDLER_H_

#include <unordered_map>

namespace eco{;

template<typename T, typename M>
class DispatchHandler
{
public:
    typedef std::function<void(M&)> HandlerFunc;
    typedef std::unordered_map<T, HandlerFunc> HandlerMap;

public:
    DispatchHandler() : m_default_handler(nullptr)
    {}

    inline void operator()(M& msg) const
    {
        dispatch((T)msg["type"].template AnyCast<int>(), msg);
    }

    inline void dispatch(const T& type, M& msg) const
    {
        auto it = m_handler_map.find(type);
        if (it != m_handler_map.end()) {
            it->second(msg);
        }
        else if (m_default_handler) {
            m_default_handler(msg);
        }
        else {
            std::cout << "dispatch unknown message type: " << type << std::endl;
        }
    }

    void set_dispatch(const T& type, HandlerFunc& handler)
    {
        m_handler_map[type] = handler;
    }

    void set_default(HandlerFunc& handler)
    {
        m_default_handler = handler;
    }

private:
    HandlerMap  m_handler_map;
    HandlerFunc m_default_handler;
};

}
#endif
