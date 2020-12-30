#ifndef __TEST_TCPCLIENT_H__
#define __TEST_TCPCLIENT_H__

#include <stdio.h>
#include <iostream>
#include <string.h>
#include <thread>
#include <memory>
#include "utility/clientsocket.h"
#include "utility/address.h"
#include "utility/state.h"

using namespace std;
using namespace utility;

class test_tcpclient {
public:
    test_tcpclient()
        : m_tcpclient(nullptr)
        , m_rec_thread(nullptr)
    {
        //windows used
        qsocket::start();
    }

    virtual ~test_tcpclient()
    {
        m_state.none();
        m_rec_thread->join();
        if (m_tcpclient != nullptr) {
            delete m_tcpclient;
            m_tcpclient = nullptr;
        }

        //windows used
        qsocket::clean();
    }
public:
    void test()
    {
        m_tcpclient = new clientsocket();
        address addr;
        addr.host_name = "127.0.0.1";
        addr.port = "8889";
        m_tcpclient->connect_server(addr);



        string data;
        m_tcpclient->send_data(data);

        m_rec_thread = std::make_shared<std::thread>(std::bind(&test_tcpclient::run, this));
        while (1) {
            q_sleep(1000);
        }

    }

    void run()
    {
        m_state.ok();
        while (m_state.isok()) {
            string rsp;
            int len = m_tcpclient->receive_data(rsp);
            cout << len << endl;

        }
    }
private:
    clientsocket *m_tcpclient;

    utility::state m_state;
    std::shared_ptr<std::thread> m_rec_thread;
};

#endif
