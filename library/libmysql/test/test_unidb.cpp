/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: test_unidb.cpp
Version: 1.0
Date: 2016.1.13

History:
ericsheng     2016.4.13   1.0     Create
******************************************************************************/

#include "test_unidb.h"
#include "mysql/dbscope.h"
#include <string>
#include <sstream>
#include <iostream>
using namespace std;
using namespace database;

test_unidb::test_unidb()
    : db_(NULL)
{
}

test_unidb::~test_unidb()
{

}

void test_unidb::start_mysql_db()
{

    char buffer[2048];
    int record_count = 2;

    unidb_param param;
    param.host = "192.168.4.44";
    param.port = 3306;
    param.user = "risk";
    param.password = "risk";
    param.charset = "utf8";
    param.database_name = "future_risk";
    param.create_database_if_not_exists = false;
    param.recreate_database_if_exists = false;
    db_  = new unidb("mysql");
    unidb_conn* conn = NULL;

    int ret = db_->open(param);
    if (ret != 0) {
        printf("open database error:%d\n", ret);
        goto end;
    }

    conn = new unidb_conn(db_);
    if (!conn->init_conn()) {
        printf("initialize unidb conn failed:%d\n", conn->get_errno());
        goto end;
    }

    /* create table */
    sprintf(buffer, "%s", "CREATE TABLE IF NOT EXISTS `hqetf` ( \
            `id` bigint(11) NOT NULL AUTO_INCREMENT, \
            `date` varchar(255) DEFAULT NULL, \
            `time` varchar(255) DEFAULT NULL, \
            `date_time` varchar(255) DEFAULT NULL, \
            `avr_price` double DEFAULT 0, \
            `price` double DEFAULT 0, \
            `deal_amount` bigint(11) DEFAULT 0, \
            `itime` bigint(11) DEFAULT 0, \
            PRIMARY KEY (`id`) \
        ) ENGINE=INNODB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC");
    if (!conn->execute(buffer)) {
        printf("create table failed\n");
    }

    /* delete all records */
    if (!conn->execute("delete from `hqetf`")) {
        printf("delete from hqetf failed\n");
    }

    //conn->set_auto_commit(false);

    /* insert records */
    for (int i = 0; i < record_count; i++) {
        sprintf(buffer, "INSERT INTO hqetf (avr_price,deal_amount,date) VALUES (%f, %d, '%s')",
            i + 3900.0, i, "2017-04-21");
    }

    conn->begin_transaction();
    for (int i = 0; i < record_count; i++) {
        sprintf(buffer, "INSERT INTO hqetf (avr_price,deal_amount,date) VALUES (%f, %d, '%s')",
            i + 4900.0, i, "2017-04-21");
        conn->execute(buffer);
    }
    conn->rollback();

    for (int i = 0; i < record_count; i++) {
        sprintf(buffer, "INSERT INTO hqetf (avr_price,deal_amount,date) VALUES (%f, %d, '%s')",
            i + 5900.0, i, "2017-04-21");
        conn->execute(buffer);
    }

    conn->begin_transaction();
    for (int i = 0; i < record_count; i++) {
        sprintf(buffer, "INSERT INTO hqetf (avr_price,deal_amount,date) VALUES (%f, %d, '%s')",
            i + 6900.0, i, "2017-04-21");
        conn->execute(buffer);
    }
    conn->commit();

    /* query records */
    if (conn->query("select * from `hqetf`")) {
        int record_id = 0;
        while (conn->fetch_row()) {
            printf("id:%d avr_price:%lf, deal_amount:%ld, date:%s\n",
                    record_id, conn->get_double("avr_price"),
                    conn->get_long("deal_amount"),
                    conn->get_string("date"));
        }
    }

    printf("end of dbtest\n");
    getchar();

end:
    if (conn != NULL) {
        conn->release_conn();
        delete conn;
        conn = NULL;
    }
    if (db_ != NULL) {
        delete db_;
        db_ = NULL;
    }
}

void test_unidb::start_mysql_db_pool()
{
    unidb_param param;
    param.host = "192.168.4.44";
    param.port = 3306;
    param.user = "risk";
    param.password = "risk";
    param.charset = "utf8";
    param.database_name = "w_test";
    param.create_database_if_not_exists = false;
    param.recreate_database_if_exists = false;

    string type = "mysql";
    tradepool.init(param, 2, type);
    dbscope db_scope(tradepool);
    db_instance* dbconn = db_scope.get_db_conn();
#if 0
    int record_count = 2;
    char buffer[2048];
    /* create table */
    sprintf(buffer, "%s", "CREATE TABLE IF NOT EXISTS `hqetf` ( \
            `id` bigint(11) NOT NULL AUTO_INCREMENT, \
            `date` varchar(255) DEFAULT NULL, \
            `time` varchar(255) DEFAULT NULL, \
            `date_time` varchar(255) DEFAULT NULL, \
            `avr_price` double DEFAULT 0, \
            `price` double DEFAULT 0, \
            `deal_amount` bigint(11) DEFAULT 0, \
            `itime` bigint(11) DEFAULT 0, \
            PRIMARY KEY (`id`) \
        ) ENGINE=MyISAM AUTO_INCREMENT=1 DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC");
    if (!dbconn->_conn->execute(buffer)) {
        printf("create table failed\n");
    }

    /* delete all records */
    if (!dbconn->_conn->execute("delete from `hqetf`")) {
        printf("delete from hqetf failed\n");
    }

    //conn->set_auto_commit(false);

    /* insert records */
    for (int i = 0; i < record_count; i++) {
        sprintf(buffer, "INSERT INTO hqetf (avr_price,deal_amount,date) VALUES (%f, %d, '%s')",
                i + 3900.0, i, "2017-04-21");
        dbconn->_conn->execute(buffer);
    }

    dbconn->_conn->begin_transaction();
    for (int i = 0; i < record_count; i++) {
        sprintf(buffer, "INSERT INTO hqetf (avr_price,deal_amount,date) VALUES (%f, %d, '%s')",
            i + 4900.0, i, "2017-04-21");
        dbconn->_conn->execute(buffer);
    }
    dbconn->_conn->rollback();

    for (int i = 0; i < record_count; i++) {
        sprintf(buffer, "INSERT INTO hqetf (avr_price,deal_amount,date) VALUES (%f, %d, '%s')",
            i + 5900.0, i, "2017-04-21");
        dbconn->_conn->execute(buffer);
    }

    dbconn->_conn->begin_transaction();
    for (int i = 0; i < record_count; i++) {
        sprintf(buffer, "INSERT INTO hqetf (avr_price,deal_amount,date) VALUES (%f, %d, '%s')",
            i + 6900.0, i, "2017-04-21");
        dbconn->_conn->execute(buffer);
    }
    dbconn->_conn->commit();
#endif

    /* query records */
    if (dbconn->_conn->query("select * from t_instrument")) {
        int affect = dbconn->_conn->affected_rows();
        cout << affect << endl;
        while (dbconn->_conn->fetch_row()) {
            printf("instrumentid:%s volumemultiple:%ld, pricetick:%f, optionstype:%c\n",
                dbconn->_conn->get_string("instrumentid"),
                dbconn->_conn->get_long("volumemultiple"),
                dbconn->_conn->get_double("pricetick"),
                dbconn->_conn->get_string("optionstype")[0]);
            break;
        }
    }

    printf("end of dbtest\n");
    getchar();

}
