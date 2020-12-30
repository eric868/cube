/*****************************************************************************
 libmysql Copyright (c) 2020. All Rights Reserved.

 FileName: dbscope.h
 Version: 1.0
 Date: 2018.7.2

 History:
 eric     2018.7.2   1.0     Create
 ******************************************************************************/

#ifndef DBSCOPE_H_
#define DBSCOPE_H_
#include "unidbpool.h"

namespace database
{

class COMMON_API dbscope
{
public:
    explicit dbscope(db_conn_pool &pool_);
    virtual ~dbscope();
    db_instance* get_db_conn();
private:
    db_conn_pool &tradepool_;
    db_instance* conn_;
};

}

#endif
