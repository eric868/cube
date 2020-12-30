/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: asio_library.cpp
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.1.13   1.0     Create
******************************************************************************/

#include "asio_library.h"
#include "asio_server.h"
#include "asio_client.h"

namespace asio 
{
    asio_server_api * create_asio_server(int port,
        asio_message_notify * ptr_notify)
	{
        return new asio_server(port, ptr_notify);
	}

    asio_client_api * create_asio_client(const char *ip, int port,
        asio_message_notify * ptr_notify)
	{
        return new asio_client(ip, port, ptr_notify);
	}

    void delete_asio_server(asio_server_api * ptr_obj)
    {
        if (ptr_obj) {
            delete ptr_obj;
            ptr_obj = NULL;
		}
	}

    void delete_asio_client(asio_client_api * ptr_obj)
	{
        if (ptr_obj) {
            delete ptr_obj;
            ptr_obj = NULL;
		}
	}
}