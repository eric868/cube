/*****************************************************************************
VBase Copyright (c) 2015. All Rights Reserved.

FileName: test_util.cpp
Version: 1.0
Date: 2015.9.16

History:
ericsheng     2015.9.15   1.0     Create
******************************************************************************/


#include "test_util.h"
#include <string>
#include <iostream>
#include <sstream>

using namespace std;
using namespace base;

test_util::test_util()
{
}

test_util::~test_util()
{
}

void test_util::test()
{
    util tmp;
    cout << tmp.local_datestamp() << endl;;

    cout << tmp.string_to_datestamp("2017-08-08") << endl;

//     cout << "before passwd encode: ";
//     std::string pwd;
//     cin >> pwd;
//     base::util::encode(const_cast<char *>(pwd.c_str()));
//     cout << "after passwd encode: " << pwd;

}


