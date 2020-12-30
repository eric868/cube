/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: asio_session.h
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#ifndef __ASIO_SESSION_H__
#define __ASIO_SESSION_H__

#ifdef WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#endif

#include <boost/asio/buffer.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <memory>
#include <iostream>

using namespace boost::asio;

namespace asio {

	class asio_session;
    typedef std::shared_ptr<asio_session> session_shared_ptr;

    class asio_message_notify
	{
	public:
        virtual void on_message_stream( const std::string& stream ) = 0;

        void set_session_ptr(session_shared_ptr ptr_socket)
		{
            m_session = ptr_socket;
		}

        session_shared_ptr get_session_ptr()
		{
            return m_session;
		}

	private:
        session_shared_ptr m_session;
	};

	class asio_session
        :public std::enable_shared_from_this<asio_session>
	{

	public:
        asio_session(boost::asio::io_service& ioservice, asio_message_notify* ptrNotify);
        virtual ~asio_session();

        std::shared_ptr<asio_session> get_self()
		{
			return shared_from_this();
		}

		void start_up();

		void send_message( const std::string &stream );

        void close_socket();

        ip::tcp::socket& get_socket() { return m_socket; }
        void set_connid(int connid) { m_connid = connid; }
        int get_connid() const { return m_connid; }
        void set_callback_error(const std::function<void(int)>& f)
        { m_callback_error = f; }

	private:
		void handle_read_head(const boost::system::error_code& error);
		void handle_read_body(const boost::system::error_code& error);
		void handle_write(const std::string &stream, const boost::system::error_code& error);

		void handle_error(const boost::system::error_code& ec);

	private:
        ip::tcp::socket m_socket;
        asio_message_notify* m_pnotify;

		const static int m_head_size = sizeof(int32_t);
        char m_head_length[m_head_size + 1];
		std::string m_message_stream;
		
        int m_connid;		
        std::function<void(int)> m_callback_error;
	};
}

#endif
