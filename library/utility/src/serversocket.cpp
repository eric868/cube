/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: serversocket.cpp
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#include "address.h"
#include "serversocket.h"
#include "clientsocket.h"
#include "applog.h"
#include <string.h>
#ifdef _MSC_VER
#include <ws2tcpip.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <errno.h>
#endif
namespace utility {

	serversocket::serversocket()
	{
		state_ = EINITIAL;
#ifdef _MSC_VER
		socket_ = INVALID_SOCKET;
#else 
		socket_ = -1;
#endif
	}

	serversocket::~serversocket()
	{
#ifdef _MSC_VER
		closesocket(socket_);
#elif defined __GNUG__
		close(socket_);
#endif
	}

	int serversocket::bind_addr(const address & addr, bool is_udp)
	{
		if (state_ != EINITIAL) {
			return -1;
		}
#ifdef _MSC_VER
		struct addrinfo *result = NULL, *ptr = NULL, hints;

		int iResult = 0;
		ZeroMemory(&hints, sizeof (hints));
		hints.ai_family = AF_INET;
		//set addinfo according protocol type
		if (is_udp) {
			hints.ai_socktype = SOCK_DGRAM;
			hints.ai_protocol = IPPROTO_UDP;
		} else {
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_protocol = IPPROTO_TCP;
		}

		hints.ai_flags = AI_PASSIVE;

		iResult = getaddrinfo(NULL, addr.port.c_str(), &hints, &result);
		if (iResult != 0) {
			state_ = EINVALID;
			DEBUG_PRINT1("getaddrinfo failed: %d\n", iResult);
			errcode_handle(iResult, "getaddrinfo", __FUNCTION__);
			return -1;
		}

		socket_ = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (socket_ == INVALID_SOCKET) {
			state_ = EINVALID;
			DEBUG_PRINT1("Error at socket(): %ld\n", WSAGetLastError());
			errcode_handle(WSAGetLastError(), "socket", __FUNCTION__);
			freeaddrinfo(result);
			return -1;
		}

		int size = 1;
		if (!is_udp) {
			iResult = setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, (char*)&size, (socklen_t)sizeof(size));
			if (iResult == -1) {
				errcode_handle(errno, "setsockopt", __FUNCTION__);
			}
		}
		// Setup the TCP listening socket
		iResult = ::bind(socket_, result->ai_addr, (int)result->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			state_ = EINVALID;
			int err_code = WSAGetLastError();
			DEBUG_PRINT1("bind failed with error: %d\n", err_code);
			errcode_handle(err_code, "bind", __FUNCTION__);
			freeaddrinfo(result);
			closesocket(socket_);
			return -1;
		}
		freeaddrinfo(result);
#elif defined __GNUG__
		struct sockaddr_in serv_addr;
		if (is_udp) {
			socket_ = socket(AF_INET, SOCK_DGRAM, 0);
		} else {
			socket_ = socket(AF_INET, SOCK_STREAM, 0);
		}

		if(socket_ < 0) {
			state_ = EINVALID;       
			errcode_handle(h_errno, "bind", __FUNCTION__); 
			return -1;
		}
		bzero((char *) &serv_addr, sizeof(serv_addr));
		int portno = atoi(addr.port.c_str());
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = INADDR_ANY;
		serv_addr.sin_port = htons(portno);
		if (bind(socket_, (struct sockaddr *) &serv_addr, 
			sizeof(serv_addr)) < 0) {
			state_ = EINVALID;       
			errcode_handle(h_errno, "bind", __FUNCTION__);
			return -1;
		}
#endif
		state_ = EBIND;
		return 0;
	}

	int serversocket::listen_connection(int conn_num) 
	{
		if (state_ != EBIND && state_ != ELISTEN) {
			return -1;
		}
#ifdef _MSC_VER
		if (listen(socket_, conn_num) == SOCKET_ERROR) {
			state_ = EINVALID;
			errcode_handle(WSAGetLastError(), "listen", __FUNCTION__);
			closesocket(socket_);
			//WSACleanup();
			return -1;
		}
#elif defined __GNUG__
		if(listen(socket_, conn_num )) {
			state_ = EINVALID; 
			errcode_handle(h_errno, "listen", __FUNCTION__);
			//修改说明，不用shutdown无法关闭阻塞的socket
			shutdown(socket_, SHUT_RDWR);
			//close(socket_);
			return -1;
		}
#endif
		state_ = ELISTEN;
		return 0;
	}

	clientsocket* serversocket::accept_client() 
	{
		if (state_ != ELISTEN) {
			return 0;
		}
#ifdef _MSC_VER
		SOCKET new_socket;
		new_socket = INVALID_SOCKET;
		// Accept a client socket
		new_socket = accept(socket_, NULL, NULL);
		if (new_socket == INVALID_SOCKET) {
			int err_code = WSAGetLastError();
			DEBUG_PRINT1("accept failed: %d\n", err_code);
			APP_LOG(applog::LOG_CRITICAL) << __FUNCTION__ << "accept failed";
			errcode_handle(err_code, "accept", __FUNCTION__);
			closesocket(socket_);
			return 0;
		} else {
			clientsocket* cl = new clientsocket();
			cl->socket_ = new_socket;
			cl->state_ = clientsocket::ECONNECT;

			fd_set read_set;
			FD_ZERO(&read_set);
			FD_SET(new_socket, &read_set);

			return cl;
		}
#elif defined __GNUG__
		struct sockaddr_in cli_addr;
		socklen_t client;
		int new_socket = accept(socket_, (struct sockaddr*) &cli_addr, & client);
		if(new_socket < 0) {
			errcode_handle(h_errno, "accept", __FUNCTION__);
			return 0;
		} else {
			clientsocket* cl = new clientsocket();
			cl->socket_ = new_socket;
			cl->state_ = clientsocket::ECONNECT;

			fd_set read_set;
			FD_ZERO(&read_set);  
			FD_SET(new_socket, &read_set);

			return cl;
		} 
#endif
	}
}

