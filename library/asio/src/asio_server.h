/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: asio_server.h
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#ifndef __ASIO_SERVER_H__
#define __ASIO_SERVER_H__

#include <unordered_map>
#include <list>
#include "asio_library.h"
#include "boost/thread.hpp"
#include "boost/thread/mutex.hpp"
namespace asio 
{
	const int MAX_CONNECT_NUM = 65536;

	class asio_server : 
        public asio_server_api
	{
	public:
        asio_server(int port, asio_message_notify* ptr_notify);
        virtual ~asio_server();

		void start_up();
        void send_message(session_shared_ptr ptr_session, const std::string &stream);

	private:
        session_shared_ptr create_session();
        void handle_accept(const boost::system::error_code& error, 
            session_shared_ptr session);
		void stop_accept();
        void client_callback_error(int connid);

        void check_ioservice_run();
        void ioservice_run();
	private:
		io_service m_ioservice;
		ip::tcp::acceptor m_acceptor;

        boost::mutex m_mutex;//对下面两个容器加锁
        std::unordered_map< int, session_shared_ptr > m_mapclients;
		std::list<int> m_listconnd;

        //io_service run的线程
        std::shared_ptr<boost::thread> m_io_service_thread;

        asio_message_notify* m_ptr_notify;
	};

}
#endif


