/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: clientsocket.h
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#ifndef __UTILITY_CLIENTSOCKET_H__
#define __UTILITY_CLIENTSOCKET_H__

#include "socket.h"

namespace utility {

	struct COMMON_API address;

	class COMMON_API clientsocket 
		: public qsocket {
	public:
		enum CLIENT_SOCKET_STATE{
			EINITIAL,
			ECONNECT,
			ECLOSE,
			EINVALID,
		};
		friend class serversocket;
		clientsocket();
		virtual ~clientsocket();
	
		int connect_server(const address & addr);
		int send_data(const string& data);
		int send_rawdata( const char* ptr_data, int ndata_bytes );
		int receive_data(string& output_str);   
		CLIENT_SOCKET_STATE state() const { return state_; }
	private:    
		CLIENT_SOCKET_STATE state_;

	};
}
#endif
