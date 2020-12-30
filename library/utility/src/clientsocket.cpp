/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: clientsocket.cpp
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#include "clientsocket.h"
#include "address.h"
#include "applog.h"

#ifdef _MSC_VER
#include <ws2tcpip.h>
#else
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <errno.h>
#endif

namespace utility {

//DO NOT MODIFY
#define	MAX_REV_BUF_SIZE 1024
#define	REC_LEN_MAX 16

	clientsocket::clientsocket()
	{
		state_ = EINITIAL;
#ifdef _MSC_VER
		socket_ = INVALID_SOCKET;
#else
		socket_ = -1;
#endif
	}

	clientsocket::~clientsocket()
	{
		if (state_ == ECONNECT) {
#ifdef _MSC_VER
			int iResult = shutdown(socket_, SD_SEND);
			if (iResult == SOCKET_ERROR) {
				DEBUG_PRINT1("shutdown failed: %d\n", WSAGetLastError());
				closesocket(socket_);
				return;
			}
			closesocket(socket_);
#else
			close(socket_);
#endif
		}
	}

	int clientsocket::connect_server(const address & addr)
	{
		if(state_ != EINITIAL) { 
			return -1;
		}
#ifdef _MSC_VER

		struct addrinfo *result = NULL, *ptr = NULL, hints;

		int iResult=0;
		int err_code =0;

		ZeroMemory( &hints, sizeof(hints) );
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		// Resolve the server address and port
		iResult = getaddrinfo(addr.host_name.c_str(), addr.port.c_str(), &hints, &result);

#define RETRY_TIMES  3
		if ( iResult != 0 ) {  
			state_ = EINVALID;
			err_code = WSAGetLastError();
			switch(err_code)
			{
			case WSATRY_AGAIN:
				{
					int try_count  =1;
					while(try_count++ <= RETRY_TIMES && iResult && err_code == WSATRY_AGAIN) {
						iResult= getaddrinfo(addr.host_name.c_str(), addr.port.c_str(), &hints, &result);
						if(iResult) {
							err_code = WSAGetLastError();
						} else {
							break;
						}
					}
					if(iResult) {
						APP_LOG(applog::LOG_ERROR) << "clientsocket::connect_server, getaddrinfo: failed after retry, err_code:" << err_code;
					}
					break;
				}
			case WSAEINVAL:
				{
					break;
				}
			default:
				{
					errcode_handle(err_code, "getaddrinfo", "clientsocket::connect_server");
				}
			}

			return -1;
		}

		// Attempt to connect to an address until one succeeds
		for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {
			socket_ = socket(ptr->ai_family, ptr->ai_socktype, 
				ptr->ai_protocol);
			if (socket_ == INVALID_SOCKET) {
				err_code = WSAGetLastError();
				switch(err_code)
				{			
				case WSAEAFNOSUPPORT:
					{
						break;
					}			

				case WSAEINVAL:
					{
						break;
					}	

				default:
					{
						//Error is handled by  commonErrCodeHandle()
						errcode_handle(err_code, "socket", "QClientSocket::connectServer");
					}
				}
				freeaddrinfo(result);
				return -1;
			}

			// Connect to server.
			iResult = connect( socket_, ptr->ai_addr, (int)ptr->ai_addrlen);
			if (iResult == SOCKET_ERROR) {
				err_code = WSAGetLastError();
				switch(err_code)
				{
				case WSAEAFNOSUPPORT:
					{
						break;
					}
				case WSAEFAULT:
					{
						break;
					}
				case WSAEINVAL:
					{
						break;
					}
				default:
					{
						errcode_handle(err_code, "connect", "clientsocket::connect_server");
					}
				}
				closesocket(socket_);
				socket_ = INVALID_SOCKET;
				continue;
			}
			break;
		}

		freeaddrinfo(result);

		if (socket_ == INVALID_SOCKET) {
			DEBUG_PRINT0("Unable to connect to server!\n");
			state_ = EINVALID;
			return -1;
		}
#elif defined __GNUG__
		int portno;
		struct sockaddr_in serv_addr;
		struct hostent *server;

		portno = atoi(addr.port.c_str());
		socket_ = socket(AF_INET, SOCK_STREAM, 0);

		if (socket_ < 0) {
			state_ = EINVALID;
			errcode_handle(h_errno, "connect", "clientsocket::connect_server");
			return -1;
		}
		server = gethostbyname(addr.host_name.c_str());
		if (server == NULL) {
			state_ = EINVALID;
			errcode_handle(h_errno, "connect", "clientsocket::connect_server");
			return -1;
		}
		bzero((char *) &serv_addr, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		bcopy((char *)server->h_addr, 
			(char *)&serv_addr.sin_addr.s_addr,
			server->h_length);
		serv_addr.sin_port = htons(portno);
		int err;
		if ((err = connect(socket_,(struct sockaddr *) &serv_addr,sizeof(serv_addr))) < 0) {
			state_ = EINVALID;
			APP_LOG(applog::LOG_ERROR) <<  "connection error code" << err;
			return -1;
		}
#endif
		state_ = ECONNECT;
		return 0;

	}

	int clientsocket::send_data(const string& data) {
		if(state_ != ECONNECT) {
			return -1;
		}       

		//add length at the head
		char LEN_HEAD[REC_LEN_MAX+1] = {0};
		sprintf(LEN_HEAD, "%16d", data.length());
		string datawithlen = LEN_HEAD;
		datawithlen += data;

#ifdef _MSC_VER
		int iResult = send( socket_, datawithlen.c_str(), datawithlen.size(), 0 );
#elif defined __GNUG__
		int iResult = write(socket_, datawithlen.c_str(), datawithlen.size()); 
#endif

#ifdef _MSC_VER
		if (iResult == SOCKET_ERROR) {
			int err_code = WSAGetLastError();
			closesocket(socket_);
#elif defined __GNUG__
		if (iResult < 0) {
			int err_code = h_errno;      
			close(socket_);
#endif
			state_ = EINVALID;
			DEBUG_PRINT1("send failed with error: %d\n", err_code);
			errcode_handle(err_code, "send", "clientsocket::send_data");
			return -1;
		}           

		return 0;
	}

	int clientsocket::send_rawdata(const char* ptr_data, int ndata_bytes)
	{
		if(state_ != ECONNECT) {
			return -1;
		}       

#ifdef _MSC_VER
		int iResult = send(socket_, ptr_data, ndata_bytes, 0);
#elif defined __GNUG__
		int iResult = write(socket_, ptr_data, ndata_bytes); 
#endif

#ifdef _MSC_VER
		if (iResult == SOCKET_ERROR) {
			int err_code = WSAGetLastError();
			closesocket(socket_);
#elif defined __GNUG__
		if (iResult < 0) {
			int err_code = h_errno;      
			close(socket_);
#endif
			state_ = EINVALID;
			DEBUG_PRINT1("send failed with error: %d\n", err_code);
			errcode_handle(err_code, "send", "QClientSocket::sendData");
			return -1;
		}           

		return iResult;
	}

	int clientsocket::receive_data(string& output_str) {
		if(state_ != ECONNECT) {
			return -1;
		}
		int err_code = 0;
		output_str.clear();
		bool first_recv = true;
		char rev_buf[MAX_REV_BUF_SIZE];
		int target_len = 0;
		int next_recv_len = REC_LEN_MAX;
		int n = MAX_REV_BUF_SIZE - 1;
		int have_recv_len = 0;
			
		do {
			memset(rev_buf, 0, sizeof(char)*MAX_REV_BUF_SIZE);
			if (!first_recv) {
				next_recv_len = min(target_len + REC_LEN_MAX - have_recv_len, MAX_REV_BUF_SIZE - 1);
			}
#ifdef _MSC_VER
			n = recv(socket_, rev_buf, next_recv_len, 0);
#elif defined __GNUG__
			n = read(socket_, rev_buf, next_recv_len);
#endif
			if (n > 0) {
				//DEBUG_PRINT1("Bytes received: %d\n", n);
			} else if (n == 0) {
				DEBUG_PRINT0("Connection closed\n");
				return -1;
			} else {
#ifdef _MSC_VER
				err_code = WSAGetLastError();
#elif defined __GNUG__
				err_code = h_errno;
#endif
				state_ = EINVALID;
				DEBUG_PRINT1("recv failed with error: %d\n", err_code);
				errcode_handle(err_code, "recv", "clientsocket::receive_data");
				return -1;

			}
			if (first_recv) {
				int ret = n;
				while (n < REC_LEN_MAX) {
#ifdef _MSC_VER
					ret = recv(socket_, rev_buf + n, next_recv_len - n, 0);
#elif defined __GNUG__
					ret = read(socket_, rev_buf + n, nextRecvLen - n);
#endif
					if ( ret > 0 ) {
						//DEBUG_PRINT1("Bytes received: %d\n", ret);
					} else if ( ret == 0 ) {
						DEBUG_PRINT0("Connection closed\n");
						return -1;
					}
					n += ret;
				}
				target_len = atoi(rev_buf);
				first_recv = false;
			} else {
				output_str.append(rev_buf, n);
			}

			have_recv_len = have_recv_len + n;
		} while (have_recv_len < target_len + REC_LEN_MAX);

		return have_recv_len - REC_LEN_MAX;
	}
}
