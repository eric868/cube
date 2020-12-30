/*****************************************************************************
 Nautilus Database Copyright (c) 2015. All Rights Reserved.

 FileName: dbmysql.cpp
 Version: 1.0
 Date: 2015.9.5

 History:
 database     2015.9.5   1.0     Create
 ******************************************************************************/

#include "dbmysql.h"
#include "common.h"
#include "base/base.h"
#include "base/trace.h"
#include <stdio.h>

namespace database
{
    /*********************************************************************/

    const char* mysql_null_string = "";

    db_mysql::db_mysql()
        : is_open_(false)
        , conn_count_(0)
        , last_error_(NAUT_DATABASE_S_OK)
    {

    }

    db_mysql::~db_mysql()
    {
        close();
    }

    int db_mysql::open(const unidb_param& params)
    {
        if (is_open_) {
            return NAUT_DATABASE_S_OK;
        }

        params_ = params;

        last_error_ = NAUT_DATABASE_S_OK;

        if (params_.recreate_database_if_exists) {
            last_error_ = recreate_database_if_exists(params_.database_name.c_str());
            CHECK_LABEL(last_error_, end);
        }

        if (params_.create_database_if_not_exists) {
            last_error_ = create_database_if_not_exists(params_.database_name.c_str());
            CHECK_LABEL(last_error_, end);
        }

        if (mysql_init(&mysql_) == NULL) {
            TRACE_ERROR("database", NAUT_DATABASE_E_INIT_MYSQL_FAILED,
                "initialize mysql failed, error:%s", mysql_error(&mysql_));
            ASSIGN_AND_CHECK_LABEL(last_error_, NAUT_DATABASE_E_INIT_MYSQL_FAILED, end);
        }

        if (mysql_real_connect(&mysql_, params_.host.c_str(),
            params_.user.c_str(), params_.password.c_str(),
            params_.database_name.c_str(), params_.port, NULL, 0) == NULL) {
            TRACE_ERROR("database", NAUT_DATABASE_E_CONNECT_TO_MYSQL_FAILED,
                "connect to mysql failed, error:%s", mysql_error(&mysql_));
            ASSIGN_AND_CHECK_LABEL(last_error_, NAUT_DATABASE_E_CONNECT_TO_MYSQL_FAILED, end);
        }

    end:
        if (BSUCCEEDED(last_error_)) {
            is_open_ = true;
        }
        return last_error_;
    }

    int db_mysql::close()
    {
        if (conn_count_ > 0) {
            TRACE_ERROR("database", NAUT_DATABASE_E_ALL_CONN_SHOULD_RELEASE,
                "all connections should be released before close database.");
            return NAUT_DATABASE_E_ALL_CONN_SHOULD_RELEASE;
        }

        if (is_open_) {
            mysql_close(&mysql_);
            is_open_ = false;
        }

        return NAUT_DATABASE_S_OK;
    }

    bool db_mysql::is_open()
    {
        return is_open_;
    }

    bool db_mysql::connected()
    {
        int ret = mysql_ping(&mysql_);
        if (ret != 0) {
            TRACE_ERROR("database", NAUT_DATABASE_E_PING_MYSQL_FAILED,
                "ping mysql failed, error:%d,%s", ret, mysql_error(&mysql_));
            return false;
        }
        return true;
    }

    std::string db_mysql::host_info()
    {
        if (!is_open_) {
            TRACE_WARNING("database", "the database has not been initialized");
            return "";
        }
        return mysql_get_host_info(&mysql_);
    }

    std::string db_mysql::server_info()
    {
        if (!is_open_) {
            TRACE_WARNING("database", "the database has not been initialized");
            return "";
        }
        return mysql_get_server_info(&mysql_);
    }

    std::string db_mysql::charset()
    {
        if (!is_open_) {
            TRACE_WARNING("database", "the database has not been initialized");
            return "";
        }
        return mysql_character_set_name(&mysql_);
    }

    std::string db_mysql::engine_type() const
    {
        return "mysql";
    }

    const unidb_param& db_mysql::params() const
    {
        return params_;
    }

    int db_mysql::create_database_if_not_exists(const char* database_name)
    {
        assert(database_name != NULL && strlen(database_name) > 0);

        MYSQL mysql;
        char buffer[1024];
        int ret = 0;

        last_error_ = NAUT_DATABASE_S_OK;

        if (mysql_init(&mysql) == NULL) {
            TRACE_ERROR("database", NAUT_DATABASE_E_INIT_MYSQL_FAILED,
                "initialize mysql failed, error:%s", mysql_error(&mysql));
            ASSIGN_AND_CHECK_LABEL(last_error_, NAUT_DATABASE_E_INIT_MYSQL_FAILED, end);
        }

        if (mysql_real_connect(&mysql, params_.host.c_str(),
            params_.user.c_str(), params_.password.c_str(),
            "", params_.port, NULL, 0) == NULL) {
            TRACE_ERROR("database", NAUT_DATABASE_E_CONNECT_TO_MYSQL_FAILED,
                "connect to mysql failed, error:%s", mysql_error(&mysql));
            ASSIGN_AND_CHECK_LABEL(last_error_, NAUT_DATABASE_E_CONNECT_TO_MYSQL_FAILED, end);
        }

        sprintf(buffer, "CREATE DATABASE IF NOT EXISTS %s", database_name);
        ret = mysql_real_query(&mysql, buffer, strlen(buffer));
        if (ret != 0) {
            TRACE_ERROR("database", NAUT_DATABASE_E_MYSQL_EXECUTE_SQL_FAILED,
                "execute sql '%s' failed, error:%d,%s", buffer, ret, mysql_error(&mysql));
            ASSIGN_AND_CHECK_LABEL(last_error_, NAUT_DATABASE_E_MYSQL_EXECUTE_SQL_FAILED, end);
        }

    end:
        mysql_close(&mysql);
        return last_error_;
    }

    int db_mysql::recreate_database_if_exists(const char* database_name)
    {
        assert(database_name != NULL && strlen(database_name) > 0);

        MYSQL mysql;
        char buffer[1024];
        int ret = 0;

        last_error_ = NAUT_DATABASE_S_OK;

        if (mysql_init(&mysql) == NULL) {
            TRACE_ERROR("database", NAUT_DATABASE_E_INIT_MYSQL_FAILED,
                "initialize mysql failed, error:%s", mysql_error(&mysql));
            ASSIGN_AND_CHECK_LABEL(last_error_, NAUT_DATABASE_E_INIT_MYSQL_FAILED, end);
        }

        if (mysql_real_connect(&mysql, params_.host.c_str(),
            params_.user.c_str(), params_.password.c_str(),
            "", params_.port, NULL, 0) == NULL) {
            TRACE_ERROR("database", NAUT_DATABASE_E_CONNECT_TO_MYSQL_FAILED,
                "connect to mysql failed, error:%s", mysql_error(&mysql));
            ASSIGN_AND_CHECK_LABEL(last_error_, NAUT_DATABASE_E_CONNECT_TO_MYSQL_FAILED, end);
        }

        sprintf(buffer, "DROP DATABASE IF EXISTS %s", database_name);
        ret = mysql_real_query(&mysql, buffer, strlen(buffer));
        if (ret != 0) {
            TRACE_ERROR("database", NAUT_DATABASE_E_MYSQL_EXECUTE_SQL_FAILED,
                "execute sql '%s' failed, error:%d,%s", buffer, ret, mysql_error(&mysql));
            ASSIGN_AND_CHECK_LABEL(last_error_, NAUT_DATABASE_E_MYSQL_EXECUTE_SQL_FAILED, end);
        }

        sprintf(buffer, "CREATE DATABASE IF NOT EXISTS %s", database_name);
        ret = mysql_real_query(&mysql, buffer, strlen(buffer));
        if (ret != 0) {
            TRACE_ERROR("database", NAUT_DATABASE_E_MYSQL_EXECUTE_SQL_FAILED,
                "execute sql '%s' failed, error:%d,%s", buffer, ret, mysql_error(&mysql));
            ASSIGN_AND_CHECK_LABEL(last_error_, NAUT_DATABASE_E_MYSQL_EXECUTE_SQL_FAILED, end);
        }

    end:
        mysql_close(&mysql);
        return last_error_;
    }

    void db_mysql::add_connection(db_mysql_conn* conn)
    {
        base_fetch_and_inc(&conn_count_);
    }

    void db_mysql::release_connection(db_mysql_conn* conn)
    {
        base_fetch_and_dec(&conn_count_);
    }

    /*********************************************************************/

    db_mysql_conn::db_mysql_conn(unidb_base* db)
        : unidb_conn_base(db)
        , db_(db)
        , result_(NULL)
        , row_(NULL)
        , is_transaction_(false)
        , num_cols_(0)
        , initialized_(false)
        , last_error_(NAUT_DATABASE_S_OK)
        , last_error_inner_(0)
    {
        assert(db != NULL);
    }

    db_mysql_conn::~db_mysql_conn()
    {
        release_conn();
    }

    bool db_mysql_conn::init_conn()
    {
        if (initialized_) {
            return true;
        }

        assert(db_ != NULL);

        LABEL_SCOPE_START;

        last_error_ = NAUT_DATABASE_S_OK;
        last_error_inner_ = 0;
        slast_error_inner_ = "";

        if (mysql_init(&mysql_) == NULL) {
            TRACE_ERROR("database", NAUT_DATABASE_E_INIT_MYSQL_FAILED,
                "initialize mysql failed, error:%s", mysql_error(&mysql_));
            ASSIGN_AND_CHECK_LABEL(last_error_, NAUT_DATABASE_E_INIT_MYSQL_FAILED, end);
        }

        last_error_ = real_connect();
        CHECK_LABEL(last_error_, end);

        LABEL_SCOPE_END;

    end:
        if (BSUCCEEDED(last_error_)) {
            initialized_ = true;
        }
        return (last_error_ == NAUT_DATABASE_S_OK);
    }

    bool db_mysql_conn::release_conn()
    {
        if (initialized_) {
            commit();

            free_result();
            last_error_inner_ = 0;
            slast_error_inner_ = "";

            mysql_close(&mysql_);
            initialized_ = false;
        }
        return false;
    }

    bool db_mysql_conn::connected()
    {
        if (!initialized_) {
            TRACE_WARNING("database", "mysql connection is not initialized or initialize failed");
            return false;
        }

        int ret = mysql_ping(&mysql_);
        if (ret) {
            TRACE_ERROR("database", NAUT_DATABASE_E_PING_MYSQL_FAILED,
                "ping mysql failed, error:%d,%s", ret, mysql_error(&mysql_));
            last_error_ = NAUT_DATABASE_E_PING_MYSQL_FAILED;
            return false;
        }
        return true;
    }

    bool db_mysql_conn::reconnect()
    {
        last_error_ = real_connect();

        return BSUCCEEDED(last_error_);
    }

    int db_mysql_conn::real_connect()
    {
        int ret = NAUT_DATABASE_S_OK;

        unidb_param params = db_->params();

        if (mysql_real_connect(&mysql_, params.host.c_str(),
            params.user.c_str(), params.password.c_str(),
            params.database_name.c_str(), params.port, NULL, 0) == NULL) {
            TRACE_ERROR("database", NAUT_DATABASE_E_CONNECT_TO_MYSQL_FAILED,
                "connect to mysql failed, error:%s", mysql_error(&mysql_));
            ASSIGN_AND_CHECK_LABEL(ret, NAUT_DATABASE_E_CONNECT_TO_MYSQL_FAILED, end);
        }

        ret = mysql_set_character_set(&mysql_, params.charset.c_str());
        if (ret != 0) {
            TRACE_ERROR("database", NAUT_DATABASE_E_MYSQL_SET_CHARSET_FAILED,
                "set mysql charset failed:%d,%s", ret, mysql_error(&mysql_));
            ASSIGN_AND_CHECK_LABEL(ret, NAUT_DATABASE_E_MYSQL_SET_CHARSET_FAILED, end);
        }

        ret = mysql_autocommit(&mysql_, true);
        if (ret != 0) {
            TRACE_ERROR("database", NAUT_DATABASE_E_MYSQL_SET_AUTOCOMMIT_FAILED,
                "mysql set auto commit failed, error:%d,%s", ret, mysql_error(&mysql_));
            ASSIGN_AND_CHECK_LABEL(ret, NAUT_DATABASE_E_MYSQL_SET_AUTOCOMMIT_FAILED, end);
        }

    end:
        return ret;
    }

    bool db_mysql_conn::begin_transaction()
    {
        if (!initialized_) {
            TRACE_WARNING("database", "mysql connection is not initialized or initialize failed");
            return false;
        }

        last_error_inner_ = 0;
        slast_error_inner_ = "";

        const char* sql = "begin";
        int ret = mysql_real_query(&mysql_, sql, strlen(sql));
        if (ret != 0) {
            last_error_inner_ = mysql_errno(&mysql_);
            slast_error_inner_ = mysql_error(&mysql_);
            if (!connected() && reconnect()) {
                ret = mysql_real_query(&mysql_, sql, strlen(sql));
                if (ret != 0) {
                    last_error_inner_ = mysql_errno(&mysql_);
                    slast_error_inner_ = mysql_error(&mysql_);
                }
            }
        }
        if (ret != 0) {
            TRACE_ERROR("database", NAUT_DATABASE_E_MYSQL_BEGIN_TRANSACTION_FAILED,
                "begin transaction failed, error: %d,%d,%s", ret, last_error_inner_, slast_error_inner_.c_str());
            last_error_ = NAUT_DATABASE_E_MYSQL_BEGIN_TRANSACTION_FAILED;
        } else {
            is_transaction_ = true;
        }

        return (ret == 0);
    }

    bool db_mysql_conn::commit()
    {
        if (!initialized_) {
            TRACE_WARNING("database", "mysql connection is not initialized or initialize failed");
            return false;
        }

        int ret = mysql_commit(&mysql_);
        if (ret != 0) {
            TRACE_ERROR("database", NAUT_DATABASE_E_MYSQL_COMMIT_FAILED,
                "mysql commit transaction failed, error:%d,%s", ret, mysql_error(&mysql_));
            last_error_ = NAUT_DATABASE_E_MYSQL_COMMIT_FAILED;
        }

        is_transaction_ = false;
        return (ret == 0);
    }

    bool db_mysql_conn::rollback()
    {
        if (!initialized_) {
            TRACE_WARNING("database", "mysql connection is not initialized or initialize failed");
            return false;
        }

        int ret = mysql_rollback(&mysql_);
        if (ret != 0) {
            TRACE_ERROR("database", NAUT_DATABASE_E_MYSQL_COMMIT_FAILED,
                "mysql rollback transaction failed, error:%d,%s", ret, mysql_error(&mysql_));
            last_error_ = NAUT_DATABASE_E_MYSQL_ROLLBACK_FAILED;
        }

        is_transaction_ = false;
        return (ret == 0);
    }

    bool db_mysql_conn::set_auto_commit(bool auto_commit)
    {
        if (!initialized_) {
            TRACE_WARNING("database", "mysql connection is not initialized or initialize failed");
            return false;
        }

        int ret = mysql_autocommit(&mysql_, auto_commit);
        if (ret != 0) {
            TRACE_ERROR("database", NAUT_DATABASE_E_MYSQL_SET_AUTOCOMMIT_FAILED,
                "mysql set auto commit failed, error:%d,%s", ret, mysql_error(&mysql_));
            last_error_ = NAUT_DATABASE_E_MYSQL_SET_AUTOCOMMIT_FAILED;
            return false;
        }
        return true;
    }

    bool db_mysql_conn::is_transaction()
    {
        return is_transaction_;
    }

    std::string db_mysql_conn::get_error()
    {
        switch (last_error_) {
        case NAUT_DATABASE_S_OK:
            return "NAUT_DATABASE_S_OK";
        case NAUT_DATABASE_E_ENGINE_NOT_SUPPORT:
            return "NAUT_DATABASE_E_ENGINE_NOT_SUPPORT";
        case NAUT_DATABASE_E_DATABASE_IS_NULL:
            return "NAUT_DATABASE_E_DATABASE_IS_NULL";
        case NAUT_DATABASE_E_DATABASE_CONN_IS_NULL:
            return "NAUT_DATABASE_E_DATABASE_CONN_IS_NULL";
        case NAUT_DATABASE_E_ALL_CONN_SHOULD_RELEASE:
            return "NAUT_DATABASE_E_ALL_CONN_SHOULD_RELEASE";
        case NAUT_DATABASE_E_INIT_MYSQL_FAILED:
            return "NAUT_DATABASE_E_INIT_MYSQL_FAILED";
        case NAUT_DATABASE_E_CONNECT_TO_MYSQL_FAILED:
            return "NAUT_DATABASE_E_CONNECT_TO_MYSQL_FAILED";
        case NAUT_DATABASE_E_PING_MYSQL_FAILED:
            return "NAUT_DATABASE_E_PING_MYSQL_FAILED";
        case NAUT_DATABASE_E_MYSQL_SET_AUTOCOMMIT_FAILED:
            return "NAUT_DATABASE_E_MYSQL_SET_AUTOCOMMIT_FAILED";
        case NAUT_DATABASE_E_MYSQL_COMMIT_FAILED:
            return "NAUT_DATABASE_E_MYSQL_COMMIT_FAILED";
        case NAUT_DATABASE_E_MYSQL_ROLLBACK_FAILED:
            return "NAUT_DATABASE_E_MYSQL_ROLLBACK_FAILED";
        case NAUT_DATABASE_E_MYSQL_EXECUTE_SQL_FAILED:
            return "NAUT_DATABASE_E_MYSQL_EXECUTE_SQL_FAILED";
        case NAUT_DATABASE_E_MYSQL_EXECUTE_QUERY_FAILED:
            return "NAUT_DATABASE_E_MYSQL_EXECUTE_QUERY_FAILED";
        case NAUT_DATABASE_E_MYSQL_CONN_UNINITIALIZED:
            return "NAUT_DATABASE_E_MYSQL_CONN_UNINITIALIZED";
        default:
            return "unknown error code";
        }
    }

    int db_mysql_conn::get_errno()
    {
        return last_error_;
    }

    std::string db_mysql_conn::get_error_inner()
    {
        return slast_error_inner_;
    }

    int db_mysql_conn::get_errno_inner()
    {
        return last_error_inner_;
    }

    bool db_mysql_conn::execute(const char* sql)
    {
        assert(sql != NULL && strlen(sql) > 0);

        if (!initialized_) {
            TRACE_WARNING("database", "mysql connection is not initialized or initialize failed");
            return false;
        }

        free_result();

        last_error_inner_ = 0;
        slast_error_inner_ = "";

        int ret = mysql_real_query(&mysql_, sql, strlen(sql));
        if (ret != 0) {
            last_error_inner_ = mysql_errno(&mysql_);
            slast_error_inner_ = mysql_error(&mysql_);
            if (!connected() && reconnect()) {
                ret = mysql_real_query(&mysql_, sql, strlen(sql));
                if (ret != 0) {
                    last_error_inner_ = mysql_errno(&mysql_);
                    slast_error_inner_ = mysql_error(&mysql_);
                }
            }
        }
        if (ret != 0) {
            TRACE_ERROR("database", NAUT_DATABASE_E_MYSQL_EXECUTE_SQL_FAILED,
                "execute sql '%s' failed, error:%d,%d,%s", sql, ret, last_error_inner_, slast_error_inner_.c_str());
            last_error_ = NAUT_DATABASE_E_MYSQL_EXECUTE_SQL_FAILED;
        }
        return ret == 0;
    }

    bool db_mysql_conn::query(const char* sql)
    {
        assert(sql != NULL && strlen(sql) > 0);

        if (!initialized_) {
            TRACE_WARNING("database", "mysql connection is not initialized or initialize failed");
            return false;
        }

        free_result();

        last_error_inner_ = 0;
        slast_error_inner_ = "";

        int ret = mysql_real_query(&mysql_, sql, strlen(sql));
        if (ret != 0) {
            last_error_inner_ = mysql_errno(&mysql_);
            slast_error_inner_ = mysql_error(&mysql_);
            if (!connected() && reconnect()) {
                ret = mysql_real_query(&mysql_, sql, strlen(sql));
                if (ret != 0) {
                    last_error_inner_ = mysql_errno(&mysql_);
                    slast_error_inner_ = mysql_error(&mysql_);
                }
            }
        }
        if (ret != 0) {
            TRACE_ERROR("database", NAUT_DATABASE_E_MYSQL_EXECUTE_SQL_FAILED,
                "execute sql '%s' failed, error:%d,%d,%s", sql, ret, last_error_inner_, slast_error_inner_.c_str());
            last_error_ = NAUT_DATABASE_E_MYSQL_EXECUTE_QUERY_FAILED;
            return false;
        } else {
            result_ = mysql_store_result(&mysql_);
            if (result_ != NULL) {
                MYSQL_FIELD* field = mysql_fetch_field(result_);
                int i = 1;
                while (field != NULL) {
                    if (field->name != NULL) {
                        map_fields_[field->name] = i;
                    }
                    field = mysql_fetch_field(result_);
                    i++;
                }
                num_cols_ = i - 1;
            }
        }
        return true;
    }

    long db_mysql_conn::get_count() const
    {
        if (result_ != NULL) {
            return mysql_num_rows(result_);
        }
        return 0;
    }

    long db_mysql_conn::fields_count() const
    {
        return num_cols_;
    }

    void db_mysql_conn::free_result()
    {
        if (result_ != NULL) {
            mysql_free_result(result_);
            result_ = NULL;
            map_fields_.clear();
            row_ = NULL;
        }
        num_cols_ = 0;
    }

    std::vector<std::string> db_mysql_conn::get_fields()
    {
        std::vector<std::string> vfields;
        VBASE_HASH_MAP<const char*, int, string_hash, string_compare>::iterator it = map_fields_.begin();
        while (it != map_fields_.end()) {
            vfields.push_back(it->first);
            it++;
        }
        return vfields;
    }


    bool db_mysql_conn::first()
    {
        if (result_ != NULL) {
            mysql_row_seek(result_, 0);
        }
        return false;
    }

    bool db_mysql_conn::fetch_row()
    {
        if (result_ != NULL) {
            row_ = mysql_fetch_row(result_);
            return row_ != NULL;
        }
        return false;
    }

    int db_mysql_conn::affected_rows()
    {
        return mysql_affected_rows(&mysql_);
    }

    bool db_mysql_conn::is_null(int id)
    {
        assert(id >= 0 && id < num_cols_);

        if (result_ != NULL && row_ != NULL) {
            return row_[id] ? false : true;
        }
        return true;
    }

    bool db_mysql_conn::is_null(const std::string& field_name)
    {
        return is_null(field_name.c_str());
    }

    bool db_mysql_conn::is_null(const char* field_name)
    {
        int index = map_fields_[field_name] - 1;
        if (index >= 0) {
            return is_null(index);
        }
        return true;
    }

    const char* db_mysql_conn::get_string(int id)
    {
        assert(id >= 0 && id < num_cols_);

        const char* ret = NULL;
        if (result_ != NULL && row_ != NULL) {
            ret = row_[id];
        }
        return ret == NULL ? mysql_null_string : ret;
    }

    const char* db_mysql_conn::get_string(const std::string& field_name)
    {
        return get_string(field_name.c_str());
    }

    const char* db_mysql_conn::get_string(const char* field_name)
    {
        int index = map_fields_[field_name] - 1;
        if (index >= 0) {
            return get_string(index);
        }
        return mysql_null_string;
    }

    double db_mysql_conn::get_double(int id)
    {
        assert(id >= 0 && id < num_cols_);

        if (result_ != NULL && row_ != NULL && row_[id] != NULL) {
            return atof(row_[id]);
        }
        return 0;
    }

    double db_mysql_conn::get_double(const std::string& field_name)
    {
        return get_double(field_name.c_str());
    }

    double db_mysql_conn::get_double(const char* field_name)
    {
        int index = map_fields_[field_name] - 1;
        if (index >= 0) {
            return get_double(index);
        }
        return 0;
    }

    long db_mysql_conn::get_long(int id)
    {
        assert(id >= 0 && id < num_cols_);

        if (result_ != NULL && row_ != NULL && row_[id] != NULL) {
            return atol(row_[id]);
        }
        return 0;
    }

    long db_mysql_conn::get_long(const std::string& field_name)
    {
        return get_long(field_name.c_str());
    }

    long db_mysql_conn::get_long(const char* field_name)
    {
        int index = map_fields_[field_name] - 1;
        if (index >= 0) {
            return get_long(index);
        }
        return 0;
    }

    base::int64 db_mysql_conn::get_int64(int id)
    {
        assert(id >= 0 && id < num_cols_);

        if (result_ != NULL && row_ != NULL && row_[id] != NULL) {
#ifdef OS32
            return atoll(row_[id]);
#else
            return atol(row_[id]);
#endif
        }
        return 0;
    }

    base::int64 db_mysql_conn::get_int64(const std::string& field_name)
    {
        return get_int64(field_name.c_str());
    }

    base::int64 db_mysql_conn::get_int64(const char* field_name)
    {
        int index = map_fields_[field_name] - 1;
        if (index >= 0) {
            return get_int64(index);
        }
        return 0;
    }
}
