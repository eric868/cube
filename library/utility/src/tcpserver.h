/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: tcpserver.h
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#ifndef __UTILITY_TCPSERVER_H__
#define __UTILITY_TCPSERVER_H__

#include <map>
#include "address.h"
#include "serversocket.h"
#include "qthread.h"
#include "shareptr.h"
#include "session.h"
#include "mutex.h"
namespace utility 
{

class COMMON_API tcpserver 
	: public thread_runner 
{
public:
	// Default server max connection client count.[1024]
	static unsigned int DefaultMaxClient;
	// Default buffer size [64*1024]
	static unsigned int DefaultBufferSize;	
	// Max datagrame size [64*1024]
	static unsigned int MaxDatagramSize;

    tcpserver(const string& hostname, const string& port, tcp_notify& notify,
        unsigned int max_client = DefaultMaxClient);
    virtual ~tcpserver();

	serversocket* get_serversocket() const { return server_socket_; }

	unsigned int get_capacity() const { return max_client_; }
	unsigned int get_client_count() const { return client_count_; }
	bool is_run() const { return is_run_; }

	virtual void start();
	virtual void stop();

	virtual void close_all_client();
	virtual void close_client(session* session); 

	virtual void send_all_client(const string& datagrame);
	virtual void send_data(session* session, const string& datagrame);
    virtual void send_data(unsigned int clientid, const string& datagrame);
	virtual void run() { start(); }

private:
	void accept_connection();

private:
	string hostname_;
	string port_;
	unsigned int max_client_;
	unsigned int client_count_;
	serversocket* server_socket_;
	map<session_id, session*> session_map_;
	mutex mutex_;
	bool is_run_;

    tcp_notify& m_notify;
};

typedef shareptr<tcpserver> tcpserver_ptr;

} 

#endif
