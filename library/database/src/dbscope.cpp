/*****************************************************************************
Nautilus Database Copyright (c) 2017. All Rights Reserved.

FileName: dbscope.cpp
Version: 1.0
Date: 2017.3.31

History:
shengkaishan     2017.3.31   1.0     Create
******************************************************************************/


#include "dbscope.h"

namespace database
{
dbscope::dbscope(db_conn_pool &pool_)
        :tradepool_(pool_),
        conn_(0)
{};

dbscope::~dbscope()
{
    if (conn_){
        tradepool_.retconn(conn_);
    }
}

db_instance* dbscope::get_db_conn()
{
    conn_ = tradepool_.getconn();
    return conn_;
}

}
