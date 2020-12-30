/*****************************************************************************
Nautilus Database Copyright (c) 2017. All Rights Reserved.

FileName: dbscope.h
Version: 1.0
Date: 2017.3.31

History:
shengkaishan     2017.3.31   1.0     Create
******************************************************************************/

#ifndef __NAUT_DATABASE_DBSCOPE_H__
#define __NAUT_DATABASE_DBSCOPE_H__
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

} /* namespace order_manager */

#endif /* MODULES_SIGMA_TM_STOCK_SOURCE_ORDER_MANAGER_SRC_DBCONNECTIONKEEPPER_H_ */
