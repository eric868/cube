/*****************************************************************************
 libmysql Copyright (c) 2020. All Rights Reserved.

 FileName: dbscope.cpp
 Version: 1.0
 Date: 2018.7.2

 History:
 eric     2018.7.2   1.0     Create
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
        conn_->_conn->free_result();
        tradepool_.retconn(conn_);
    }
}

db_instance* dbscope::get_db_conn()
{
    conn_ = tradepool_.getconn();
    return conn_;
}

}
