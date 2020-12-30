/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: test_server.h
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.4.13   1.0     Create
******************************************************************************/

#ifndef __TEST_CURL_H__
#define __TEST_CURL_H__

#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <curl/easy.h>
#include "httpclient.h"
using namespace std;

class test_curl
{
public:
    test_curl(){};
    ~test_curl(){};

public:
    void test()
    {
        std::string result;
        CHttpClient tt;
        tt.Get("www.baidu.com", result);

        cout << result << endl;
    }
};

#endif
