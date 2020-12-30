/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: main.cpp
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.4.13   1.0     Create
******************************************************************************/

#include "test_unidb.h"

int main(int argc, char* argv[])
{
    test_unidb t;
    //t.start_mysql_db();
    t.start_mysql_db_pool();

    return 0;
}



