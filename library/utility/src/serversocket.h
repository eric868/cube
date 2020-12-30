/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: serversocket.h
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#ifndef __UTILITY_SERVERSOCKET_H__
#define __UTILITY_SERVERSOCKET_H__

#include "socket.h"

namespace utility {
	struct COMMON_API address;
	class COMMON_API clientsocket;

	class COMMON_API serversocket 
		: public qsocket 
	{
	public:
		enum SERVER_SOCKET_STATE{
			EINITIAL,
			EBIND,
			ELISTEN,
			EINVALID,
		};
		serversocket();
		virtual ~serversocket();
		int bind_addr(const address & addr, bool is_udp = false);
		int listen_connection(int conn_num);
		clientsocket* accept_client();

	private:    
		SERVER_SOCKET_STATE state_;
	};
}
#endif
