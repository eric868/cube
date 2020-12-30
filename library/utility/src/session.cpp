/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: session.cpp
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#include "session.h"
#include "tcpserver.h"

#include <sstream>

namespace utility
{

    session::session(clientsocket* cli_socket, tcpserver* tcpserver, tcp_notify& notify)
        : m_notify(notify)
    {
        if (cli_socket == NULL) {
            return;
        }
        tcpserver_ = tcpserver;
        client_socket_ = cli_socket;
        id_.set_id(cli_socket->get_socket_handle());
        std::cout << "session connection successful, session_id: "
            << id_.get_id() << std::endl;
    }

    session::~session() {
        if (client_socket_ != NULL) {
            delete client_socket_;
        }
        thread_runner::after_terminate();
    }

    session& session::operator=(const session& other) {
        if (this == &other) {
            return *this;
        }

        id_ = other.id_;
        datagram_ = other.datagram_;
        client_socket_ = other.client_socket_;
        exit_type_ = other.exit_type_;

        return *this;
    }

    void session::run() {
        while (true) {
            int res = client_socket_->receive_data(datagram_);
            //cout << datagram_.size() << endl;
            if (client_socket_->state() == clientsocket::ECLOSE ||
                client_socket_->state() == clientsocket::EINVALID ||
                res == -1) {
                tcpserver_->close_client(this);
                delete client_socket_;
                //thread_runner::after_terminate();
                break;
            }
            m_notify.on_recv_data(client_socket_->get_socket_handle(), datagram_);
            //send_data(datagram_);
        }
    }

    void session::send_data(const string& datagrame) {
        client_socket_->send_data(datagrame);
    }

    void session::send_all_session(const string& datagrame) {
        tcpserver_->send_all_client(datagrame);
    }

    ostream& operator<<(ostream& output, const session& session) {
        output << "session:" << session.get_sessionid().get_id()
            << endl;

        return output;
    }

    bool operator==(const session& rhs, const session& lhs) {
        return (rhs.get_sessionid() == lhs.get_sessionid());
    }



}
