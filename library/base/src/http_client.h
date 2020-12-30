/*****************************************************************************
 VBase Copyright (c) 2015. All Rights Reserved.

 FileName: http_client.h
 Version: 1.0
 Date: 2016.03.29

 History:
 david wang     2016.03.29   1.0     Create
 ******************************************************************************/

#ifndef _HTTP_CLIENT_H_
#define _HTTP_CLIENT_H_

#include "base.h"
#include "trace.h"
#include "sigslot.h"
#include "event2/event.h"
#include "event2/buffer.h"
#include "event2/http.h"
#include "event2/http_struct.h"
#include "event2/keyvalq_struct.h"

#include <errno.h>
#include <stdlib.h>
#include <string>

using std::string;

using namespace std;

namespace base
{
#define DATA_SIZE_LEN 4024
#define URL_SIZE_LEN 5512

// (default)
#define HTTP_CONTENT_TYPE_URL_ENCODED   "application/x-www-form-urlencoded"
// (use for files: picture, mp3, tar-file etc.)
#define HTTP_CONTENT_TYPE_FORM_DATA     "multipart/form-data"
// (use for plain text)
#define HTTP_CONTENT_TYPE_TEXT_PLAIN    "text/plain"

#define REQUEST_POST_FLAG               2
#define REQUEST_GET_FLAG                3

struct http_request_get {
    struct evhttp_uri *uri;
    struct event_base *base;
    struct evhttp_connection *cn;
//    struct evhttp_request *req;
};

struct http_request_post {
    struct evhttp_uri *uri;
    struct event_base *base;
    struct evhttp_connection *cn;
//    struct evhttp_request *req;
    char *content_type;
    char *post_data;
};

typedef void (*return_body_cb)(char* des, size_t size);

class COMMON_API http_client
{
public:
    http_client();
    ~http_client();
    // url = "http://172.16.239.93:8899/base/truck/delete"
    // data = "name=winlin&code=1234"
    int http_request_post_msg(const char *url, const char* data);
    // url = "http://127.0.0.1?name=winlin"
    // data = NULL
    int http_request_get_msg(const char *url, const char* data);

    void set_body_post_cb(return_body_cb body_cb);
    void set_body_get_cb(return_body_cb body_cb);

protected:
    void *start_http_requset(struct event_base* base, const char *url, int req_get_flag, \
                                                    const char *content_type, const char* data);

private:
    static void http_requset_post_cb(struct evhttp_request *req, void *arg);
    static void http_requset_get_cb(struct evhttp_request *req, void *arg);

    static int start_url_request(struct http_request_get *http_req, int req_get_flag);
    void *http_request_new(struct event_base* base, const char *url, int req_get_flag, \
                           const char *content_type, const char* data);
    void http_request_free(struct http_request_get *http_req_get, int req_get_flag);

private:
    static inline void print_request_head_info(struct evkeyvalq *header)
    {
        struct evkeyval *first_node = header->tqh_first;
        while (first_node) {
            TRACE_DEBUG(COMM_TRACE_TAG,"key:%s  value:%s", first_node->key, first_node->value);
            first_node = first_node->next.tqe_next;
        }
    }

    static inline void print_uri_parts_info(const struct evhttp_uri * http_uri)
    {
        TRACE_DEBUG(COMM_TRACE_TAG,"scheme:%s", evhttp_uri_get_scheme(http_uri));
        TRACE_DEBUG(COMM_TRACE_TAG,"host:%s", evhttp_uri_get_host(http_uri));
        TRACE_DEBUG(COMM_TRACE_TAG,"path:%s", evhttp_uri_get_path(http_uri));
        TRACE_DEBUG(COMM_TRACE_TAG,"port:%d", evhttp_uri_get_port(http_uri));
        TRACE_DEBUG(COMM_TRACE_TAG,"query:%s", evhttp_uri_get_query(http_uri));
        TRACE_DEBUG(COMM_TRACE_TAG,"userinfo:%s", evhttp_uri_get_userinfo(http_uri));
        TRACE_DEBUG(COMM_TRACE_TAG,"fragment:%s", evhttp_uri_get_fragment(http_uri));
    }

private:
    static return_body_cb m_body_post_cb_;
    static return_body_cb m_body_get_cb_;
    char m_data_buf_[DATA_SIZE_LEN];
    char m_url_buf_[URL_SIZE_LEN];

};

}

#endif //_HTTP_CLIENT_H_

