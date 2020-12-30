/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: asio_client.h
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#ifndef __ASIO_CLIENT_H__
#define __ASIO_CLIENT_H__
#include "asio_library.h"
#include "boost/thread.hpp"

namespace asio 
{
	class asio_client 
		: public asio_client_api
	{
	public:
        asio_client(const char *ip, int port,
            asio_message_notify* ptr_notify = NULL);
        virtual ~asio_client();

		void start_up( void );
		void send_message( const std::string &stream );
	private:
		void handle_connect(const boost::system::error_code& error);
		void handle_connect_error(const boost::system::error_code& error);
		void handle_rw_error(int connid);
		void check_connect();
        void thread_reconnect();
        bool is_connected() const { return m_bconnected; };
		
        void check_ioservice_run();
        void ioservice_run();
	private:
		boost::asio::io_service m_ioservice;
        ip::tcp::endpoint m_endpoint;

        session_shared_ptr m_session;
		bool m_bconnected;
        //专门检测重连的线程
		std::shared_ptr<boost::thread> m_chk_thread; 
		
        //io_service run的线程
        std::shared_ptr<boost::thread> m_io_service_thread;

        asio_message_notify* m_ptr_notify;
	};

}

#endif

