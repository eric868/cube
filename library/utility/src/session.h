/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: session.h
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#ifndef __UTILITY_SESSION_H__
#define __UTILITY_SESSION_H__

#include "session_id.h"
#include "clientsocket.h"
#include "qthread.h"
#include <iostream>
namespace utility
{
    class tcp_notify
    {
    public:
        virtual size_t on_recv_data(const unsigned int clientid, const string& buf) = 0;
    };

    class COMMON_API tcpserver;
    class COMMON_API session
        : public thread_runner
    {
    public:
        // Exit type enumerator defined
        enum EXITTYPE {
            NORMALEXIT,
            EXCEPTIONEXIT,
        };

        session(clientsocket* cli_socket, tcpserver* tcpserver, tcp_notify& notify);
        virtual ~session();
        session& operator=(const session& other);

        string get_datagram() const { return datagram_; }
        void set_datagram(const string& datagram) { datagram_ = datagram; }
        EXITTYPE get_exittype() { return exit_type_; }
        void set_exittype(EXITTYPE exit_type) { exit_type_ = exit_type; }

        session_id get_sessionid() const { return id_; }
        clientsocket* get_clientsocket() const { return client_socket_; }

        // Exceute session and receive client data.
        virtual void run();
        virtual void send_data(const string& datagrame);
        virtual void send_all_session(const string& datagrame);

    private:
        session_id id_;
        // Client send to server datagrame
        // Note: sometimes datagrame maybe no complete
        string datagram_;
        clientsocket* client_socket_;
        tcpserver* tcpserver_;
        EXITTYPE exit_type_;

        tcp_notify& m_notify;
    };

    COMMON_API ostream& operator<<(ostream& output, const session& session);
    COMMON_API bool operator==(const session& rhs, const session& lhs);

}

#endif
