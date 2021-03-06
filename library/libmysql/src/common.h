/*****************************************************************************
 libmysql Copyright (c) 2020. All Rights Reserved.

 FileName: common.h
 Version: 1.0
 Date: 2018.7.2

 History:
 eric     2018.7.2   1.0     Create
 ******************************************************************************/

#ifndef COMMON_H_
#define COMMON_H_

#include <string>
#include <ctime>
#include <stdio.h>

inline std::string local_time_string()
{
    struct tm ti = {0};
    time_t now = time(NULL);
#ifdef _MSC_VER
    localtime_s(&ti, &now);
#else
    localtime_r(&now, &ti);
#endif

    char time_buf[20];
    sprintf(time_buf, "%02d:%02d:%02d", ti.tm_hour, ti.tm_min, ti.tm_sec);
    return time_buf;
}

namespace database
{

enum DATABASE_ERROR
{
    NAUT_DATABASE_S_OK = 0,
    NAUT_DATABASE_E_ENGINE_NOT_SUPPORT = 2001,

    NAUT_DATABASE_E_DATABASE_IS_NULL = 2002,
    NAUT_DATABASE_E_DATABASE_CONN_IS_NULL = 2003,
    NAUT_DATABASE_E_ALL_CONN_SHOULD_RELEASE = 2004,

    NAUT_DATABASE_E_INIT_MYSQL_FAILED = 2005,
    NAUT_DATABASE_E_CONNECT_TO_MYSQL_FAILED = 2006,
    NAUT_DATABASE_E_PING_MYSQL_FAILED = 2007,
    NAUT_DATABASE_E_MYSQL_SET_AUTOCOMMIT_FAILED = 2008,
    NAUT_DATABASE_E_MYSQL_BEGIN_TRANSACTION_FAILED = 2009,
    NAUT_DATABASE_E_MYSQL_COMMIT_FAILED = 2010,
    NAUT_DATABASE_E_MYSQL_ROLLBACK_FAILED = 2011,
    NAUT_DATABASE_E_MYSQL_EXECUTE_SQL_FAILED = 2012,
    NAUT_DATABASE_E_MYSQL_EXECUTE_QUERY_FAILED = 2013,
    NAUT_DATABASE_E_MYSQL_CONN_UNINITIALIZED = 2014,
    NAUT_DATABASE_E_MYSQL_SET_CHARSET_FAILED = 2015,

    NAUT_DATABASE_E_INIT_ORACLE_FAILED = 3005,
    NAUT_DATABASE_E_CONNECT_TO_ORACLE_FAILED = 3006,
    NAUT_DATABASE_E_PING_ORACLE_FAILED = 3007,
    NAUT_DATABASE_E_ORACLE_SET_AUTOCOMMIT_FAILED = 3008,
    NAUT_DATABASE_E_ORACLE_BEGIN_TRANSACTION_FAILED = 3009,
    NAUT_DATABASE_E_ORACLE_COMMIT_FAILED = 3010,
    NAUT_DATABASE_E_ORACLE_ROLLBACK_FAILED = 3011,
    NAUT_DATABASE_E_ORACLE_EXECUTE_SQL_FAILED = 3012,
    NAUT_DATABASE_E_ORACLE_EXECUTE_QUERY_FAILED = 3013,
    NAUT_DATABASE_E_ORACLE_CONN_UNINITIALIZED = 3014,
    NAUT_DATABASE_E_ORACLE_SET_CHARSET_FAILED = 3015,
};

}

#endif /* COMMON_H_ */
