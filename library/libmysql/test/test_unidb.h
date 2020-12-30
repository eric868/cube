/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: test_unidb.h
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.4.13   1.0     Create
******************************************************************************/

#ifndef __TEST_DATABASE_H__
#define __TEST_DATABASE_H__


#include "mysql/unidbpool.h"
#include "mysql/unidb.h"
#include <stdio.h>


class test_unidb
{
public:
    test_unidb();
    virtual ~test_unidb();

public:
    void start_mysql_db();
    void start_mysql_db_pool();
private:
    database::db_conn_pool tradepool;
    database::unidb *db_;
};

#endif
