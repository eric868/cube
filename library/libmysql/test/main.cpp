/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: main.cpp
Version: 1.0
 Date: 2018.7.2

 History:
 eric     2018.7.2   1.0     Create
******************************************************************************/

#include "liblog/log.h"
#include "test_unidb.h"

int main(int argc, char* argv[])
{
    light::Logger::Instance().InitPersistLog(false, true);
    light::Logger::Instance().InitConsoleLog();
    light::Logger::Instance().Filter(debug);

    test_unidb t;
    t.start_mysql_db();
    //t.start_mysql_db_pool();

    return 0;
}



