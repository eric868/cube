/*****************************************************************************
 VBase Copyright (c) 2015. All Rights Reserved.

 FileName: http_server.h
 Version: 1.0
 Date: 2015.10.11

 History:
 ericsheng     2015.10.11   1.0     Create
 ******************************************************************************/

#include "http_server.h"

namespace base
{

    http_server::http_server()
    {
        m_ip = "0.0.0.0";
        m_nport = 8080;
        m_httpbase = NULL;
        m_p_func_call_back = NULL;
        m_ntimeout = 60;
    }

    http_server::~http_server()
    {
        m_ip = "0.0.0.0";
        m_nport = 8080;
        m_httpbase = NULL;
        m_p_func_call_back = NULL;
        m_ntimeout = 60;
    }

    int http_server::init(std::string sip, int nport, http_call_back p_func, std::string &msg, int ntimeout)
    {
        msg = "";
        m_ip = sip;
        m_nport = nport;
        m_p_func_call_back = p_func;
        m_httpbase = event_base_new();
        m_ntimeout = ntimeout;
        if (m_httpbase == NULL) {
            msg = "httpserver::init new http base failed";
            return -1;
        }

        return 0;
    }

    void http_server::run()
    {
        struct evhttp *http;
        struct evhttp_bound_socket *handle;


        http = evhttp_new(m_httpbase);
        if (http == NULL) {
            return;
        }

        evhttp_set_gencb(http, m_p_func_call_back, NULL);

        handle = evhttp_bind_socket_with_handle(http, m_ip.c_str(), m_nport);
        if (handle == NULL) return;

        evhttp_set_timeout(http, m_ntimeout);

        event_base_dispatch(m_httpbase);
        evhttp_free(http);
        event_base_free(m_httpbase);
        m_httpbase = 0;
        return;
    }

    void http_server::stop()
    {
        if (m_httpbase) {
            event_base_loopbreak(m_httpbase);
        }
    }

    http_call_back multi_thread_httpserver::m_p_func_call_back = NULL;

    multi_thread_httpserver::multi_thread_httpserver()
    {
        m_ip = "0.0.0.0";
        m_nport = 8080;
        //m_p_func_call_back = NULL;
        m_ntimeout = 60;
        m_nthreads = 5;
        m_vec_ths.clear();
        m_vec_httpd.clear();
    }

    multi_thread_httpserver::~multi_thread_httpserver()
    {
        m_ip = "0.0.0.0";
        m_nport = 8080;
        //m_p_func_call_back = NULL;
        m_ntimeout = 60;
        m_nthreads = 5;
        m_vec_ths.clear();
        m_vec_httpd.clear();
    }

    int multi_thread_httpserver::httpserver_bindsocket(int port, int backlog)
    {
        int r, nfd;
        nfd = socket(AF_INET, SOCK_STREAM, 0);
        if (nfd < 0) {
            return -1;
        }

        int one = 1;
        r = setsockopt(nfd, SOL_SOCKET, SO_REUSEADDR, (char*)&one, sizeof(int));

        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port);

        r = bind(nfd, (struct sockaddr*)&addr, sizeof(addr));
        if (r < 0) {
            return -1;
        }
        r = listen(nfd, backlog);
        if (r < 0) {
            return -1;
        }

#ifdef _MSC_VER
        u_long iMode = 1;
        ioctlsocket(nfd, FIONBIO, &iMode);
#elif defined __GNUG__
        int flags;
        flags = fcntl(nfd, F_GETFL, 0);
        if (flags < 0)  return -1;
        if (fcntl(nfd, F_SETFL, flags | O_NONBLOCK) < 0) return -1;
#endif
        return nfd;
    }

    int multi_thread_httpserver::httpserver_start(int port, int nthreads, int backlog)
    {
        int r, i;
        int nfd = httpserver_bindsocket(port, backlog);
        if (nfd < 0) {
            return -1;
        }

        for (i = 0; i < nthreads; i++) {
            struct event_base *base = event_init();
            if (base == NULL) {
                return -1;
            }
            struct evhttp *httpd = evhttp_new(base);
            if (httpd == NULL) {
                return -1;
            }
            r = evhttp_accept_socket(httpd, nfd);
            if (r != 0) {
                return -1;
            }
            m_vec_httpd.push_back(httpd);
            evhttp_set_gencb(httpd, httpserver_handler, NULL);

            evhttp_set_timeout(httpd, m_ntimeout);
            m_vec_ths.push_back(std::make_shared<std::thread>(httpserver_dispatch, base));
        }

        return 0;
    }

    void* multi_thread_httpserver::httpserver_dispatch(void *arg)
    {
        event_base_dispatch((struct event_base*)arg);
        return NULL;
    }

    void multi_thread_httpserver::httpserver_handler(struct evhttp_request* req, void *arg)
    {
        multi_thread_httpserver::httpserver_processrequest(req);
    }

    void multi_thread_httpserver::httpserver_processrequest(struct evhttp_request *req)
    {
        m_p_func_call_back(req, NULL);
    }

    int multi_thread_httpserver::init(std::string sip, int nport, http_call_back p_func, std::string &msg, int ntimeout, int nthreads)
    {
        msg = "";
        m_ip = sip;
        m_nport = nport;
        m_p_func_call_back = p_func;
        m_ntimeout = ntimeout;
        m_nthreads = nthreads;

        return 0;
    }

    int multi_thread_httpserver::run()
    {
        int ret = httpserver_start(m_nport, m_nthreads, 10240);

        return ret;
    }

    void multi_thread_httpserver::stop()
    {
        for (int i = 0; i < (int)m_vec_ths.size(); i++) {
            m_vec_ths[i]->join();
        }

        for (int i = 0; i < (int)m_vec_httpd.size(); i++) {
            evhttp_free(m_vec_httpd[i]);
        }
    }

}
