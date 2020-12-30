/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: asio_server.cpp
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#include "asio_server.h"
#include <numeric>
#include <functional>

namespace asio {

    asio_server::asio_server(int port, asio_message_notify* ptr_notify)
		: m_acceptor(m_ioservice, ip::tcp::endpoint(ip::tcp::v4(), port))
        , m_listconnd(MAX_CONNECT_NUM)
        , m_ptr_notify(ptr_notify)	
	{
        m_listconnd.resize(MAX_CONNECT_NUM);
        std::iota(m_listconnd.begin(), m_listconnd.end(), 1);
    }
    asio_server::~asio_server()
	{
	}

    void asio_server::start_up()
	{
        session_shared_ptr new_session = create_session();

        if (new_session == nullptr) return;

		m_acceptor.async_accept(new_session->get_socket(),
            boost::bind(&asio_server::handle_accept, this,
			boost::asio::placeholders::error, new_session));

		check_ioservice_run();
	}

    void asio_server::send_message(session_shared_ptr ptr_session,
        const std::string &stream)
    {
        ptr_session->send_message(stream);
    }

    session_shared_ptr asio_server::create_session()
    {
        int connid = -1;
        {
            boost::unique_lock<boost::mutex> locker(m_mutex);
            if (m_listconnd.empty()) {
                return nullptr;
            }
            connid = m_listconnd.front();
            m_listconnd.pop_front();
        }

        session_shared_ptr session = std::make_shared<asio_session>(m_ioservice, m_ptr_notify);
        session->set_connid(connid);

        std::function<void(int)> fr = std::bind(&asio_server::client_callback_error,
            this, std::placeholders::_1);
        session->set_callback_error(fr);

        return session;
    }

    void asio_server::handle_accept(const boost::system::error_code& error, 
        session_shared_ptr session)
	{
		if (error) {
			std::cout << error.value() << " " << error.message() << std::endl;
			session->close_socket();
			stop_accept();
		} 
        else {
			{
				boost::unique_lock<boost::mutex> locker(m_mutex);
                m_mapclients.insert(std::make_pair(
                    session->get_connid(), session));
			}
            std::cout << "current connect count: " << 
                m_mapclients.size() << std::endl;

			session->start_up();

			start_up();
		}
	}

    void asio_server::stop_accept()
	{
		boost::system::error_code ec;
		m_acceptor.cancel(ec);
		m_acceptor.close(ec);
		m_ioservice.stop();
	}

    void asio_server::client_callback_error(int connid)
    {
        boost::unique_lock<boost::mutex> locker(m_mutex);
        auto it = m_mapclients.find(connid);
        if (it != m_mapclients.end()) {
            m_mapclients.erase(it);
        }

        m_listconnd.push_back(connid);
    }

    void asio_server::check_ioservice_run()
    {
        if (m_io_service_thread != nullptr) return;

        m_io_service_thread = std::make_shared<boost::thread>(
            boost::bind(&asio_server::ioservice_run, this));
    }

    void asio_server::ioservice_run()
    {
        while (1) {
            m_ioservice.run();
            boost::this_thread::sleep(
                boost::posix_time::seconds(1));
        }
        std::cout << "asio_server close" << std::endl;
    }
}
