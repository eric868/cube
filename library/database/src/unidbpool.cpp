/*****************************************************************************
Nautilus Database Copyright (c) 2015. All Rights Reserved.

FileName: dbmysql.cpp
Version: 1.0
Date: 2015.9.5

History:
database     2015.9.5   1.0     Create
******************************************************************************/

#include "unidbpool.h"
#include "base/trace.h"
#include "base/util.h"
#include "stdio.h"

namespace database
{

db_conn_pool::db_conn_pool()
{
    m_nconns = 5;
    m_vec_conn.clear();
    db_ = NULL;
}

db_conn_pool::~db_conn_pool()
{
    if(is_running()) {
    	stop();
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
		std::unique_lock<std::mutex> wl(m_mutex);
        m_vec_conn.clear();
        db_ = new unidb(m_stype);
        int ret = db_->open(m_param);
        if(ret != 0) {
            TRACE_ERROR("unidbpool",10000,"open database error:%d\n",ret);
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

int db_conn_pool::init(unidb_param param,int nconns,std::string &stype, std::string switch_time)
{
    m_param = param;
    m_nconns = nconns;
    m_stype = stype;
    m_switch_time = switch_time;

    start();

    return init_db_conn();
}

db_instance* db_conn_pool::getconn()
{
	std::unique_lock<std::mutex> wl(m_mutex);

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
	std::unique_lock<std::mutex> wl(m_mutex);

    if(dbconn == NULL) {
        return;
    }

    if(dbconn->_conn->is_transaction()) {
    	if(!dbconn->_conn->rollback()) {
    		TRACE_ERROR("unidbpool",10000,"rollback database error!\n");
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
	std::unique_lock<std::mutex> wl(m_mutex);

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
        TRACE_ERROR("unidbpool",10000,"initialize unidb conn failed:%d\n", dbconn->_conn->get_errno());
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
    TRACE_SYSTEM("unidbpool", "db_conn_pool on alarm now~~~");
    release_all_conns();
    int ret = init_db_conn();
    if(ret != 0) {
        TRACE_ERROR("unidbpool", 10000, "init db pool failed :%d", ret);
    }
}

void db_conn_pool::run()
{
	while (is_running()) {
		if (base::util::local_time_string() == m_switch_time ) {
			alarm_callback();
		}
		base::util::sleep(1000);
	}
}

}

