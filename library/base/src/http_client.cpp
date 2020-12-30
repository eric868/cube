/*****************************************************************************
 VBase Copyright (c) 2015. All Rights Reserved.

 FileName: http_client.cpp
 Version: 1.0
 Date: 2016.03.29

 History:
 david wang     2016.03.29   1.0     Create
 ******************************************************************************/
#include "http_client.h"
#include <string.h>

namespace base
{

return_body_cb http_client::m_body_post_cb_ = NULL;
return_body_cb http_client::m_body_get_cb_ = NULL;

http_client::http_client()
{
    memset(m_data_buf_, 0, DATA_SIZE_LEN);
    memset(m_url_buf_, 0, URL_SIZE_LEN);
}

http_client::~http_client()
{

}

int http_client::http_request_post_msg(const char *url, const char* data)
{
    int ret = NAUT_S_OK;
    if(url == NULL ||  data == NULL) {
        TRACE_ERROR(COMM_TRACE_TAG,NAUT_COMM_E_HTTP_CLIENT_ERROR, "url or data is null");
        return NAUT_COMM_E_HTTP_CLIENT_ERROR;
    }

    memset(m_data_buf_, 0, DATA_SIZE_LEN);
    memset(m_url_buf_, 0, URL_SIZE_LEN);
    if(strlen(url) >= URL_SIZE_LEN || strlen(data) >= DATA_SIZE_LEN) {
        TRACE_ERROR(COMM_TRACE_TAG,NAUT_COMM_E_HTTP_CLIENT_ERROR, "length of url is %d >= URL_SIZE_LEN(%d) url: [%s]; "
                        "or length of data is %d >= DATA_SIZE_LEN(%d) data: [%s]",
                    strlen(url), URL_SIZE_LEN, url, strlen(data), DATA_SIZE_LEN, data);
        return NAUT_COMM_E_HTTP_CLIENT_ERROR;
    }

    memcpy(m_data_buf_, data, strlen(data));
    memcpy(m_url_buf_, url, strlen(url));

    event_base *base = event_base_new();
    if(base == NULL) {
        TRACE_ERROR(COMM_TRACE_TAG,NAUT_COMM_E_HTTP_CLIENT_ERROR, "base new error is");
        return NAUT_COMM_E_HTTP_CLIENT_ERROR;
    }

    struct http_request_post *http_req_post = (http_request_post*)start_http_requset(base,
                                                                 m_url_buf_,
                                                                 REQUEST_POST_FLAG,
                                                                 HTTP_CONTENT_TYPE_URL_ENCODED,
                                                                 m_data_buf_);

    if(http_req_post != NULL) {
        event_base_dispatch(base);
    }
    if(http_req_post != NULL) {
        http_request_free((struct http_request_get *)http_req_post, REQUEST_POST_FLAG);
    }
    event_base_free(base);

    return ret;
}

int http_client::http_request_get_msg(const char *url, const char* data)
{
    int ret = NAUT_S_OK;
    if(url == NULL ||  data == NULL) {
        TRACE_ERROR(COMM_TRACE_TAG,NAUT_COMM_E_HTTP_CLIENT_ERROR, "url or data is null");
        return NAUT_COMM_E_HTTP_CLIENT_ERROR;
    }

//    char m_url_buf_[URL_SIZE_LEN];
    memset(m_url_buf_, 0, URL_SIZE_LEN);

    string outstr = url;
    outstr += data;
    if(outstr.length() >= URL_SIZE_LEN) {
        TRACE_ERROR(COMM_TRACE_TAG,NAUT_COMM_E_HTTP_CLIENT_ERROR, "length of outstr is %d >= URL_SIZE_LEN(%d) outstr: [%s]",
                    outstr.length(), URL_SIZE_LEN, outstr.c_str());
        return NAUT_COMM_E_HTTP_CLIENT_ERROR;
    }
    memcpy(m_url_buf_, outstr.c_str(), outstr.length());
    TRACE_SYSTEM(COMM_TRACE_TAG, "outstr:%s", m_url_buf_);

    event_base *base = event_base_new();
    if(base == NULL) {
        TRACE_ERROR(COMM_TRACE_TAG,NAUT_COMM_E_HTTP_CLIENT_ERROR, "base new error is %d >= URL_SIZE_LEN(%d) outstr: [%s]",
                    outstr.length(), URL_SIZE_LEN, outstr.c_str());
        return NAUT_COMM_E_HTTP_CLIENT_ERROR;
    }
    struct http_request_get *http_req_get = (http_request_get *)start_http_requset(base,
                                                               m_url_buf_,
                                                               REQUEST_GET_FLAG,
                                                               NULL,
                                                               NULL);
    if(http_req_get != NULL) {
        event_base_dispatch(base);
    }
    if(http_req_get != NULL) {
        http_request_free(http_req_get, REQUEST_GET_FLAG);
    }
    event_base_free(base);

    return ret;
}

void http_client::set_body_post_cb(return_body_cb body_cb)
{
    m_body_post_cb_ = body_cb;
}

void http_client::set_body_get_cb(return_body_cb body_cb)
{
    m_body_post_cb_ = body_cb;
}

void* http_client::start_http_requset(struct event_base* base, const char *url, int req_get_flag, \
    const char *content_type, const char* data)
{
    struct http_request_get *http_req_get = (http_request_get*)http_request_new(base, url, req_get_flag, content_type, data);
    if (http_req_get == NULL) {
        return NULL;
    }

    int ret = start_url_request(http_req_get, req_get_flag);
    if (ret == -1) {
        if (http_req_get != NULL) {
            http_request_free(http_req_get, req_get_flag);
        }
        return NULL;
    }

    return http_req_get;
}

void http_client::http_requset_post_cb(struct evhttp_request *req, void *arg)
{
    struct http_request_post *http_req_post = (struct http_request_post *)arg;
    if(req == NULL) {
        TRACE_WARNING(COMM_TRACE_TAG, "http_requset_post_cb req == NULL");
        event_base_loopbreak(http_req_post->base);
        return;
    }

    switch(req->response_code)
    {
        case HTTP_OK:
        {
            struct evbuffer* buf = evhttp_request_get_input_buffer(req);
            size_t len = evbuffer_get_length(buf);
            TRACE_SYSTEM(COMM_TRACE_TAG, "print the head info:");
            print_request_head_info(req->output_headers);

            TRACE_SYSTEM(COMM_TRACE_TAG,"len:%zu  body size:%zu", len, req->body_size);
            char *tmp = (char*)malloc(len+1);
            memcpy(tmp, evbuffer_pullup(buf, -1), len);
            tmp[len] = '\0';
            TRACE_SYSTEM(COMM_TRACE_TAG, "print the body:");
            TRACE_SYSTEM(COMM_TRACE_TAG,"HTML BODY:%s", tmp);
            if(m_body_post_cb_ != NULL) {
                m_body_post_cb_(tmp, len);
            }
            free(tmp);
            event_base_loopbreak(http_req_post->base);
            break;
        }
        case HTTP_MOVEPERM:
            TRACE_ERROR(COMM_TRACE_TAG, NAUT_COMM_E_HTTP_CLIENT_ERROR, "%s", "the uri moved permanently");
            event_base_loopbreak(http_req_post->base);
            break;
        case HTTP_MOVETEMP:
        {
//            const char *new_location = evhttp_find_header(req->input_headers, "Location");
//            struct evhttp_uri *new_uri = evhttp_uri_parse(new_location);
//            evhttp_uri_free(http_req_post->uri);
//            http_req_post->uri = new_uri;
//            start_url_request((struct http_request_get *)http_req_post, REQUEST_POST_FLAG);
//            TRACE_SYSTEM(COMM_TRACE_TAG,"new location: %s", new_location);
            TRACE_ERROR(COMM_TRACE_TAG,NAUT_COMM_E_HTTP_CLIENT_ERROR, "%s", "the uri moved permanently");
            event_base_loopbreak(http_req_post->base);
            return;
        }

        default:
            TRACE_WARNING(COMM_TRACE_TAG,"response_code : %d", req->response_code);
            event_base_loopbreak(http_req_post->base);
            return;
    }
}
void http_client::http_requset_get_cb(struct evhttp_request *req, void *arg)
{
    struct http_request_get *http_req_get = (struct http_request_get *)arg;
    if(req == NULL) {
        TRACE_WARNING(COMM_TRACE_TAG, "http_requset_get_cb req == NULL");
//        if (param->retried_times < REQ_RESEND_TIMES) {
//            resend_order_back(param);
//            event_base_loopbreak(param->base);
//            return;
//        } else {
//            order_manager::remove_order(param->order_id, ORDER_REQ_TIMEOUT, "request timeout");
//            event_base_loopbreak(param->base);
//            return;
//        }
        event_base_loopbreak(http_req_get->base);
        return;
    }

    switch(req->response_code)
    {
        case HTTP_OK:
        {
            struct evbuffer* buf = evhttp_request_get_input_buffer(req);
            size_t len = evbuffer_get_length(buf);
            TRACE_SYSTEM(COMM_TRACE_TAG, "print the head info:");
            print_request_head_info(req->output_headers);

            TRACE_SYSTEM(COMM_TRACE_TAG,"len:%zu  body size:%zu", len, req->body_size);
            char *tmp = (char*)malloc(len+1);
            memcpy(tmp, evbuffer_pullup(buf, -1), len);
            tmp[len] = '\0';
            TRACE_SYSTEM(COMM_TRACE_TAG, "print the body:");
            TRACE_SYSTEM(COMM_TRACE_TAG,"HTML BODY:%s", tmp);
            if(m_body_get_cb_ != NULL) {
                m_body_get_cb_(tmp, len);
            }
            free(tmp);

            event_base_loopbreak(http_req_get->base);
            break;
        }
        case HTTP_SERVUNAVAIL:
//            if (param->retried_times < REQ_RESEND_TIMES) {
//                resend_order_back(param);
//                event_base_loopbreak(param->base);
//                return;
//            } else {
//                order_manager::remove_order(param->order_id, ORDER_SERVER_NOT_AVAILABLE, "server not available");
//                event_base_loopbreak(param->base);
//                return;
//            }
            TRACE_ERROR(COMM_TRACE_TAG,NAUT_COMM_E_HTTP_CLIENT_ERROR, "%s", "server not available");
            event_base_loopbreak(http_req_get->base);
            break;
        case HTTP_NOTFOUND:
//            if (param->retried_times < REQ_RESEND_TIMES) {
//                resend_order_back(param);
//                event_base_loopbreak(param->base);
//            } else {
//                order_manager::remove_order(param->order_id, ORDER_SERVER_NOT_FOUND, "server not found");
//                event_base_loopbreak(param->base);
//            }
            event_base_loopbreak(http_req_get->base);
            break;
        case HTTP_MOVEPERM:
            TRACE_ERROR(COMM_TRACE_TAG,NAUT_COMM_E_HTTP_CLIENT_ERROR, "%s", "the uri moved permanently");
            event_base_loopbreak(http_req_get->base);
            break;
        default:
            TRACE_ERROR(COMM_TRACE_TAG,NAUT_COMM_E_HTTP_CLIENT_ERROR, "%s", "unknown error");
            event_base_loopbreak(http_req_get->base);
            return;
    }
}

int http_client::start_url_request(struct http_request_get *http_req, int req_get_flag)
{
    if (http_req->cn) {
        evhttp_connection_free(http_req->cn);
    }

    int port = evhttp_uri_get_port(http_req->uri);
    http_req->cn = evhttp_connection_base_new(http_req->base,
                                                   NULL,
                                                   evhttp_uri_get_host(http_req->uri),
                                                   (port == -1 ? 80 : port));
    if (!http_req->cn ){
        return -1;
    }

    evhttp_connection_set_timeout(http_req->cn, 10);

    /**
     * Request will be released by evhttp connection
     * See info of evhttp_make_request()
     */

    struct evhttp_request *ev_req = NULL;
    if (req_get_flag == REQUEST_POST_FLAG) {
        ev_req = evhttp_request_new(http_client::http_requset_post_cb, http_req);
    } else if (req_get_flag ==  REQUEST_GET_FLAG) {
        ev_req = evhttp_request_new(http_client::http_requset_get_cb, http_req);
    }

    if(ev_req == NULL) {
        return -1;
    }

    if (req_get_flag == REQUEST_POST_FLAG) {
        const char *path = evhttp_uri_get_path(http_req->uri);
        /** Set the post data */
        struct http_request_post *http_req_post = (struct http_request_post *)http_req;

        /** Set the header properties */
        evhttp_add_header(ev_req->output_headers, "Host", evhttp_uri_get_host(http_req->uri));
        evhttp_add_header(ev_req->output_headers, "Content-Type", http_req_post->content_type);
        evbuffer_add(ev_req->output_buffer, http_req_post->post_data, strlen(http_req_post->post_data));

        int res =  evhttp_make_request(http_req->cn, ev_req, EVHTTP_REQ_POST,
                            path ? path : "/");
        if(res != 0) {
            return -1;
        }
    } else if (req_get_flag == REQUEST_GET_FLAG) {
        const char *query = evhttp_uri_get_query(http_req->uri);
        const char *path = evhttp_uri_get_path(http_req->uri);
        string path_query("");
        if(path != NULL && query != NULL) {
            path_query = path;
            path_query += "?";
            path_query += query;
        } else {
            return -1;
        }

//        size_t len = (query ? strlen(query) : 0) + (path ? strlen(path) : 0) + 2;
//        char *path_query = NULL;
//
//
//        if (len > 1) {
//            path_query = (char*)calloc(len, sizeof(char));
//            memset(path_query, 0, len);
//            sprintf(path_query, "%s?%s", path, query);
//        }
        /** Set the header properties */
        evhttp_add_header(ev_req->output_headers, "Host", evhttp_uri_get_host(http_req->uri));
        evhttp_add_header(ev_req->output_headers, "Content-Type", "text/plain; charset=UTF-8");

        int res = evhttp_make_request(http_req->cn, ev_req, EVHTTP_REQ_GET,
                             path_query.c_str());

        if(res != 0) {
            return -1;
        }
    }

    return 0;
}

void* http_client::http_request_new(struct event_base* base, const char *url, int req_get_flag, \
                       const char *content_type, const char* data)
{
    int len = 0;
    if (req_get_flag == REQUEST_GET_FLAG) {
        len = sizeof(struct http_request_get);
    } else if(req_get_flag == REQUEST_POST_FLAG) {
        len = sizeof(struct http_request_post);
    }

    struct http_request_get *http_req_get = (http_request_get *)calloc(1, len);
    if(http_req_get == NULL) {
        return NULL;
    }
    memset(http_req_get, 0, len);

    http_req_get->uri = evhttp_uri_parse(url);
    print_uri_parts_info(http_req_get->uri);

    http_req_get->base = base;

    if (req_get_flag == REQUEST_POST_FLAG) {
        struct http_request_post *http_req_post = (struct http_request_post *)http_req_get;
        if (content_type == NULL) {
            content_type = HTTP_CONTENT_TYPE_URL_ENCODED;
        }
        http_req_post->content_type = strdup(content_type);

        if (data == NULL) {
            http_req_post->post_data = NULL;
        } else {
            http_req_post->post_data = strdup(data);
        }
    }

    return http_req_get;
}

void http_client::http_request_free(struct http_request_get *http_req_get, int req_get_flag)
{
    if(http_req_get->cn) {
        evhttp_connection_free(http_req_get->cn);
    }
    if(http_req_get->uri) {
        evhttp_uri_free(http_req_get->uri);
    }
    if (req_get_flag == REQUEST_GET_FLAG) {
        free(http_req_get);
    } else if(req_get_flag == REQUEST_POST_FLAG) {
        struct http_request_post *http_req_post = (struct http_request_post*)http_req_get;
        if (http_req_post->content_type) {
            free(http_req_post->content_type);
        }
        if (http_req_post->post_data) {
            free(http_req_post->post_data);
        }
        free(http_req_post);
    }
    http_req_get = NULL;
}

}
