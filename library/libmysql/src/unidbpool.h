/*****************************************************************************
 libmysql Copyright (c) 2020. All Rights Reserved.

 FileName: unidbpool.h
 Version: 1.0
 Date: 2018.7.2

 History:
 eric     2018.7.2   1.0     Create
 ******************************************************************************/

#ifndef UNIDB_POOL_H_
#define UNIDB_POOL_H_

#include <vector>
#include <string>
#include <thread>
#include <memory>
#include <mutex>
#include <atomic>
#include "base.h"
#include "unidb.h"

using namespace std;

namespace database
{
enum STATUS
{
  USE = 0,
  IDLE = 1,
  EXCEPTION = 2
};

struct db_instance
{
    unidb_conn* _conn;
    int _status;

    db_instance()
    : _conn(NULL),
    _status(IDLE)
    {
    }
};

class COMMON_API db_conn_pool
{
public:
    db_conn_pool();
    ~db_conn_pool();
public:
    int init(unidb_param param,int nconns, std::string &stype,
            std::string switch_time = "04:00:00");
    db_instance* getconn();
    void release_all_conns();
    void retconn(db_instance *dbconn);
    void alarm_callback();

    void run();
private:
    db_instance* createconn();
    int init_db_conn();
private:
    vector<db_instance*> m_vec_conn;
    int m_nconns;
    unidb* db_;

    unidb_param m_param;
    std::string m_stype;

    std::mutex m_mutex;
    std::string m_switch_time;

    std::atomic<bool> isrunning_;
    std::shared_ptr<std::thread> ptr_reconnect_;
};

}

#endif
