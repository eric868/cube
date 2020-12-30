/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: socket.h
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#ifndef __UTILITY_SOCKET_H__
#define __UTILITY_SOCKET_H__

#ifdef WIN32
#include <WinSock2.h>
#else
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#endif
//utility.h头文件必须放在WinSock2.h之后，应为里面有windows.h
#include "utility.h"
#include <string>
using namespace std;

#define SOCKET_DEBUG 1

#if SOCKET_DEBUG
#define DEBUG_PRINT0(value) printf(value)
#define DEBUG_PRINT1(format, value) printf(format, value)
#else
#define DEBUG_PRINT0(value) 
#define DEBUG_PRINT1(format, value)
#endif

namespace utility {

	enum SELECT_STATE{
		TIMEOUT = 0,
		WRITABLE = 1,
		READABLE = 2,
		WRABLE = 3,
		EXCEPT = 4,        
	};

	class COMMON_API qsocket {
	public:
		qsocket();
		static int start();
		static int clean();
		int select(int timeout);

		//false:send true:receive
		void set_buffer(bool recv_opt, int size);
		void set_unblock();

#ifdef WIN32
		SOCKET get_socket_handle() { return socket_; }
#else
		int get_socket_handle() { return socket_; }
#endif

	protected:
		static void errcode_handle(int err_code, string sock_api_name, 
			string code_contex);

		bool is_unblock;
#ifdef WIN32
		SOCKET socket_;
#else
		int socket_; 
#endif  
	};
}
#endif
