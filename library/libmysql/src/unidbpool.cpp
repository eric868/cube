/*****************************************************************************
 libmysql Copyright (c) 2020. All Rights Reserved.

 FileName: unidbpool.cpp
 Version: 1.0
 Date: 2018.7.2

 History:
 eric     2018.7.2   1.0     Create
 ******************************************************************************/

#include "unidbpool.h"
#include <chrono>
#include <stdio.h>
#include "liblog/log.h"
#include "common.h"

namespace database
{

db_conn_pool::db_conn_pool()
    : m_nconns(5)
    , db_(NULL)
    , isrunning_(false)
    , ptr_reconnect_(nullptr)
{
    m_vec_conn.clear();
}

db_conn_pool::~db_conn_pool()
{
    if(isrunning_.load()) {
        isrunning_ = false;
    }

    if (ptr_reconnect_ != nullptr && ptr_reconnect_->joinable()) {
        ptr_reconnect_->join();
        ptr_reconnect_.reset();
    }

    release_all_conns();

    if(db_) {
        delete db_;
        db_ = NULL;
    }
}

int db_conn_pool::init_db_conn()
{
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_vec_conn.clear();
        db_ = new unidb(m_stype);
        int ret = db_->open(m_param);
        if(ret != 0) {
            STLOG_ERROR << "unidbpool open database error:" << ret;
            delete db_;
            db_ = NULL;
            return ret;
        }

        for(int i=0; i<m_nconns; i++) {
            db_instance* dbconn = createconn();
            if(dbconn != NULL) {
                m_vec_conn.push_back(dbconn);
            }
        }
    }

    return m_vec_conn.size() > 0 ? 0: -1;
}

int db_conn_pool::init(unidb_param param,int nconns,std::string &stype,
        std::string switch_time)
{
    m_param = param;
    m_nconns = nconns;
    m_stype = stype;
    m_switch_time = switch_time;
    ptr_reconnect_ = std::make_shared<std::thread>(&db_conn_pool::run, this);

    return init_db_conn();
}

db_instance* db_conn_pool::getconn()
{
    std::unique_lock<std::mutex> lock(m_mutex);

    int i=0;
    for(i=0;i<(int)m_vec_conn.size();i++) {
        if(m_vec_conn[i] != NULL && m_vec_conn[i]->_status == IDLE) {
            if(m_vec_conn[i]->_conn->connected() == false) {
                m_vec_conn[i]->_status = EXCEPTION;
                continue;
            }
            m_vec_conn[i]->_status = USE;
            return m_vec_conn[i];
        }
    }

    if(i>=(int)m_vec_conn.size()) {
        for(int j=0;j<5;j++) {
            db_instance *dbconn = createconn();
            if(dbconn == NULL) {
                continue;
            }

            dbconn->_status = USE;
            m_vec_conn.push_back(dbconn);

            return dbconn;
        }
    }

    return NULL;
}

void db_conn_pool::retconn(db_instance *dbconn)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    if(dbconn == NULL) {
        return;
    }

    if(dbconn->_conn->is_transaction()) {
        if(!dbconn->_conn->rollback()) {
            STLOG_ERROR << "unidbpool rollback database error!";
        }
    }

    for(int i=0;i<(int)m_vec_conn.size();i++) {
        if(m_vec_conn[i] == dbconn) {
            m_vec_conn[i]->_status = IDLE;
        }
    }
}

void db_conn_pool::release_all_conns()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    for(int i=0;i<(int)m_vec_conn.size();i++) {
        if(m_vec_conn[i] != NULL) {
            if(m_vec_conn[i]->_conn != NULL) {
                m_vec_conn[i]->_conn->release_conn();
                delete m_vec_conn[i]->_conn;
                m_vec_conn[i]->_conn = NULL;
            }

            delete m_vec_conn[i];
            m_vec_conn[i] = NULL;
        }
    }
}

db_instance* db_conn_pool::createconn()
{
    db_instance* dbconn = new db_instance;

    dbconn->_conn = new unidb_conn(db_);

    if(!dbconn->_conn->init_conn()) {
        STLOG_ERROR << "unidbpool initialize unidb conn failed:" <<
                dbconn->_conn->get_errno();
        delete dbconn->_conn;
        dbconn->_conn = NULL;
        delete dbconn;
        dbconn = NULL;
        return NULL;
    }

    dbconn->_status = IDLE;
    
    return dbconn;
}

void db_conn_pool::alarm_callback()
{
    STLOG_INFO << "unidbpool db_conn_pool on alarm now~~~";
    release_all_conns();
    int ret = init_db_conn();
    if(ret != 0) {
        STLOG_ERROR << "unidbpool init db pool failed :" << ret;
    }
}

void db_conn_pool::run()
{
    isrunning_ = true;
    while (isrunning_.load()) {
        if (local_time_string() == m_switch_time ) {
            alarm_callback();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

}

