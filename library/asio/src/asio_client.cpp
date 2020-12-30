/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: asio_client.cpp
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#include "asio_client.h"

namespace asio {

    asio_client::asio_client(const char *ip, int port,
        asio_message_notify* ptr_notify)
        : m_endpoint(ip::tcp::endpoint(ip::address_v4::from_string(ip), port))
        , m_bconnected(false)
		, m_chk_thread(nullptr)
        , m_io_service_thread(nullptr)
        , m_ptr_notify(ptr_notify)
	{
        m_session = std::make_shared<asio_session>(m_ioservice, ptr_notify);
	}
    asio_client::~asio_client()
	{
	}

    void asio_client::start_up()
	{
        std::function<void(int)> fr = std::bind(&asio_client::handle_rw_error, this, std::placeholders::_1);
		m_session->set_callback_error(fr);

		//连接服务端 connect
        m_session->get_socket().async_connect(m_endpoint,
			boost::bind(&asio_client::handle_connect, this, boost::asio::placeholders::error));

		check_ioservice_run();
	}

    void asio_client::send_message(const std::string &stream)
    {
        while (!m_bconnected) {
            boost::this_thread::sleep(
                boost::posix_time::seconds(1));
        }

        m_session->send_message(stream);
    }

	void asio_client::handle_connect(const boost::system::error_code& error)
	{
		if ( error ) {
			if ( !is_connected() ) {
				handle_connect_error(error);
			}
			return;
		}
        std::cout << "connect ok" << std::endl;
		m_bconnected = true;
        //连接成功后发起一个异步读的操作
		m_session->start_up(); 
	}

	void asio_client::handle_connect_error( const boost::system::error_code& error )
	{
		if ( !is_connected() ) {
			std::cout << error.message() << std::endl;
		}
	}

    void asio_client::handle_rw_error(int connid)
	{
		m_bconnected = false;
		check_connect();
	}

    void asio_client::check_connect()
    {
        if (m_chk_thread != nullptr) return;

        m_chk_thread = std::make_shared<boost::thread>(
            boost::bind(&asio_client::thread_reconnect, this));
    }

    void asio_client::thread_reconnect()
	{
		while (1) {
			if ( !is_connected() ) {
				//连接服务端 connect
				m_session->get_socket().async_connect(m_endpoint,
					boost::bind(&asio_client::handle_connect, 
                    this, boost::asio::placeholders::error));
			}

			boost::this_thread::sleep(boost::posix_time::seconds(1));
		}
	}

    void asio_client::check_ioservice_run()
    {
        if (m_io_service_thread != nullptr) return;

        m_io_service_thread = std::make_shared<boost::thread>(
            boost::bind(&asio_client::ioservice_run, this));
    }

    void asio_client::ioservice_run()
	{
		while (1) {
			m_ioservice.run();
            boost::this_thread::sleep(
                boost::posix_time::seconds(1));
		}
        std::cout << "asio_client close" << std::endl;
	}
}
