#ifndef __TEST_TCPSERVER_H__
#define __TEST_TCPSERVER_H__

#include <stdio.h>
#include <iostream>

#include "utility/tcpserver.h"
using namespace std;
using namespace utility;

class notify : public tcp_notify 
{
    virtual size_t on_recv_data(const unsigned int clientid, const string& buf)
    {
        cout << buf << endl;

        return buf.size();
    }
};

class test_tcpserver {
public:
    test_tcpserver()
        : m_tcpserver(nullptr)
    {}

    virtual ~test_tcpserver()
    {
        get_thread_manager().remove_thread(*m_tcpserver);
        if (m_tcpserver != nullptr) {
            delete m_tcpserver;
            m_tcpserver = nullptr;
        }
    }
public:
    void test()
    {
        m_tcpserver = new tcpserver("127.0.0.1", "8889", m_notify);
        get_thread_manager().create_thread(*m_tcpserver);
    }
private:
    notify m_notify;
    tcpserver *m_tcpserver;
};

#endif
