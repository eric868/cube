/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: asio_session.cpp
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#include "asio_session.h"
#include <sstream>
#include <string.h>
#include <string>
//#include "utility/Thread.h"
namespace asio {

    asio_session::asio_session(io_service& ioservice, 
        asio_message_notify* ptr_notify)
        : m_socket(ioservice), 
        m_pnotify(ptr_notify)
	{
	}
    asio_session::~asio_session()
	{
	}

    void asio_session::start_up()
	{
        //三种情况下会返回：1.缓冲区满；2.transfer_at_least为真(收到特定数量字节即返回)；3.有错误发生
		boost::asio::async_read(m_socket,
            boost::asio::buffer((char*)m_head_length, m_head_size),
            boost::bind(&asio_session::handle_read_head, shared_from_this(),
			boost::asio::placeholders::error));

    }

	void asio_session::send_message( const std::string &stream )
	{
		if ( stream.length() <= 0 ) {
			std::cout << "data error" << std::endl;
			return;
		}
		//同步发送
		boost::system::error_code ec;
		write(m_socket, boost::asio::buffer(stream.c_str(), stream.length()), ec);
		if (ec != nullptr) {
			handle_error(ec);
			return;
		}
        //异步发送暂且不用
        //boost::asio::async_write(m_socket,
        //	boost::asio::buffer(stream.c_str(), stream.length()),
        //	boost::bind(&asio_session::handle_write,shared_from_this(), stream, boost::asio::placeholders::error));

	}

    void asio_session::close_socket()
	{
		boost::system::error_code ec;
		m_socket.shutdown(ip::tcp::socket::shutdown_send, ec);
		m_socket.close(ec);
	}

    void asio_session::handle_read_head(const boost::system::error_code& error)
    {
        if (!error) {
            int32_t len = 0;
            memcpy(&len, m_head_length, sizeof(int32_t));
            len = ntohl(len);
            m_message_stream.resize(len);

            boost::asio::async_read(m_socket,
                boost::asio::buffer(&m_message_stream[0], len),
                boost::bind(&asio_session::handle_read_body, shared_from_this(),
                boost::asio::placeholders::error));
        } else {
            handle_error(error);
            return;
        }
    }

    void asio_session::handle_read_body(const boost::system::error_code& error)
    {

        if (!error) {
            m_pnotify->set_session_ptr(get_self());
            //不包括heade头数据了,接收方，已经把头接掉了
            m_pnotify->on_message_stream(m_message_stream);

            start_up();
        } else {
            handle_error(error);
            return;
        }

    }

    void asio_session::handle_write(const std::string &stream,
        const boost::system::error_code& error)
    {
        if (error) {
            handle_error(error);
            return;
        }

    }

    void asio_session::handle_error(const boost::system::error_code& ec)
	{
		close_socket(); 
		std::cout << ec.message() << std::endl;
        if (m_callback_error) {
            m_callback_error(m_connid);
        }
	}

}
