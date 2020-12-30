/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: tcpserver.cpp
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#include "tcpserver.h"

namespace utility
{

	// Default server max connection client count.[1024]
	unsigned int tcpserver::DefaultMaxClient = 1024;

	// Default buffer size [64*1024]
	unsigned int tcpserver::DefaultBufferSize = 64 * 1024;

	// Max datagrame size [64*1024]
	unsigned int tcpserver::MaxDatagramSize = 64 * 1024;

    tcpserver::tcpserver(const string& hostname, const string& port, tcp_notify& notify,
		unsigned int max_client) 
		: hostname_(hostname),
		port_(port),
        m_notify(notify),
		max_client_(max_client),
		is_run_(false)
	{
		//windows used
		qsocket::start();
	}

    tcpserver::~tcpserver()
    {
        stop();
        //thread_runner::after_terminate();
    }

	void tcpserver::start() {
		if (is_run_) {
			return;
		}
		is_run_ = true;

		// recv_data_buffer_ = new char[DefaultBufferSize]; 
		server_socket_ = new serversocket();
		address addr;
		addr.host_name = hostname_;
		addr.port = port_;
		server_socket_->bind_addr(addr);
		server_socket_->listen_connection(5);
		//server_socket_->set_unblock();
		//server_socket_->select(100);

		accept_connection();
	}

	void tcpserver::stop() {
		if (!is_run_) {
			return;
		}

		is_run_ = false;
		close_all_client();
		delete server_socket_;
	}

	void tcpserver::close_all_client() 
	{
		map<session_id, session*>::iterator iter = session_map_.begin();
		for (; iter != session_map_.end(); ++iter) {
			delete iter->second;
		}

		client_count_ = 0;
		session_map_.clear();
	}

	void tcpserver::close_client(session* session) 
	{
		if (session == NULL) {
			return;
		}
		scope_mutex locker(&mutex_);
		session_map_.erase(session->get_sessionid());
		--client_count_;
		//delete session;
	}

	void tcpserver::send_all_client(const string& datagrame) {
		scope_mutex locker(&mutex_);
		map<session_id, session*>::iterator iter = session_map_.begin();
		for (; iter != session_map_.end(); ++iter) {
			iter->second->send_data(datagrame);
		}
	}

	void tcpserver::send_data(session* session, const string& datagrame) {
		if (session == NULL) {
			return;
		}
		session->send_data(datagrame);
	}

    void tcpserver::send_data(unsigned int clientid, const string& datagrame) {
        session_id key(clientid);
        auto it = session_map_.find(key);
        if (it != session_map_.end()) {
            session_map_[key]->send_data(datagrame);
        }
    }

	void tcpserver::accept_connection() {
		if (!is_run_) return;

		// Loop accept connection
		clientsocket* client = server_socket_->accept_client();
		if (client == NULL) {
			return;
		}

		// Check connection has been max count.
		if (client_count_ == max_client_) {
			client->send_data(string("Server Full"));
			delete client;
			return;
		}

		client->set_buffer(false, 10240000);
		client->set_buffer(true, 102400);
		session* session_ptr = new session(client, this, m_notify);
		session_map_[session_ptr->get_sessionid()] = session_ptr;
		++client_count_;

		thread_manager::create_thread(*session_ptr);
		//client->send_data(string("Connection Successful|"));

		//q_sleep(1000);
		accept_connection();
	}
}
