/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: asio_library.h
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#ifndef __ASIO_LIBRARY_H_
#define __ASIO_LIBRARY_H_

#ifdef WIN32
#ifdef COMMON_EXPORT
#define COMMON_API  __declspec(dllexport)
#else 
#define COMMON_API  __declspec(dllimport)
#endif  //end COMMON_EXPORT

#else	//LINUX平台，定义导出宏为空
#define COMMON_API
#endif

#include "asio_session.h"
namespace asio 
{
	class COMMON_API asio_server_api
	{
	public:
        virtual ~asio_server_api(){};

		// 在指定端口启动TCP服务器侦听
		virtual void start_up( void ) = 0;

		// 发送数据给指定的客户端
		virtual void send_message(session_shared_ptr ptr_socket, 
            const std::string &stream ) = 0;
	};

	class COMMON_API asio_client_api
	{ 
	public:
        virtual ~asio_client_api(){};

		// 启动连接TCP服务器
		virtual void start_up( void ) = 0;

		// 发送数据给指定的客户端
		virtual void send_message( const std::string &stream ) = 0;
	};

	extern "C"
	{
        COMMON_API asio_server_api * create_asio_server(int port, 
            asio_message_notify * ptr_notify);

        COMMON_API asio_client_api * create_asio_client(const char *ip, int port, 
            asio_message_notify * ptr_notify);

        COMMON_API void delete_asio_server(asio_server_api * ptr_obj);

        COMMON_API void delete_asio_client(asio_client_api * ptr_obj);
	}
}
#endif