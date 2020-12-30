/*****************************************************************************
 VBase Copyright (c) 2015. All Rights Reserved.

 FileName: httpserver.h
 Version: 1.0
 Date: 2015.10.11

 History:
 ericsheng     2015.10.11   1.0     Create
 ******************************************************************************/
 
#ifndef _HTTP_SERVER_H_
#define _HTTP_SERVER_H_

#include <event2/event.h>
#include <event2/http.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <evutil.h>
#include <event2/keyvalq_struct.h>
#include <event.h>
#include <evhttp.h>
#include <string>

#include <errno.h>
#include <string.h>
#include <fcntl.h>
#ifndef WIN32
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#endif // !WIN32

#include "thread.h"
#include <thread>
#include <memory>
#include <vector>

namespace base
{
	
typedef void(*http_call_back)(struct evhttp_request *req, void *arg);
	
class COMMON_API http_server
    : public process_thread
{
public:
    http_server();
    ~http_server();
	int init(std::string sip, int nport, http_call_back p_func, std::string &msg, int ntimeout=60);
	virtual void run();
	void stop();
public:
	std::string m_ip;
	int m_nport;
	struct event_base *m_httpbase;
	http_call_back m_p_func_call_back;
	int m_ntimeout;
};

class COMMON_API multi_thread_httpserver
{
public:
    multi_thread_httpserver();
    ~multi_thread_httpserver();
protected:
    int httpserver_bindsocket(int port, int backlog);
    int httpserver_start(int port, int nthreads, int backlog);
    static void* httpserver_dispatch(void *arg);
    static void httpserver_handler(struct evhttp_request* req, void *arg);
    static void httpserver_processrequest(struct evhttp_request *req);
public:
    int init(std::string sip, int nport, http_call_back p_func, std::string &msg, int ntimeout = 60, int nthreads = 5);
    int run();
    void stop();

private:
    std::string m_ip;
    int m_nport;
    static http_call_back m_p_func_call_back;
    int m_ntimeout;
    int m_nthreads;
    std::vector< std::shared_ptr<std::thread> > m_vec_ths;
    std::vector<struct evhttp*> m_vec_httpd;

};
}

#endif //_HTTP_SERVER_H_
