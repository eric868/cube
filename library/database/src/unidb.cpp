/*****************************************************************************
 Nautilus Database Copyright (c) 2015. All Rights Reserved.

 FileName: unidb.cpp
 Version: 1.0
 Date: 2015.9.5

 History:
 database     2015.9.5   1.0     Create
 ******************************************************************************/

#include "unidb.h"
#include "common.h"
#include "dbmysql.h"
#include "dboracle.h"
namespace database
{

#define CHECK_UNIDB(db, ret) { if (db == NULL) return ret; }
#define CHECK_UNIDB_CONN_VOID(conn) { if (conn == NULL) return; }
#define CHECK_UNIDB_CONN(conn, ret) { if (conn == NULL) return ret; }
#define SAFE_RELEASE_OBJ(obj) { if (obj != NULL) { delete obj; obj = NULL; } }

    /*********************************************************************/

    unidb::unidb(const std::string& engine_type)
        : real_db_(NULL)
        , engine_type_(engine_type)
    {
        if (engine_type_ == "mysql") {
            real_db_ = new db_mysql();
		}
		else if (engine_type_ == "oracle") {
			real_db_ = new db_oracle();
		}
        else {
            assert(false);
        }
    }

    unidb::~unidb()
    {
        SAFE_RELEASE_OBJ(real_db_);
    }

    int unidb::open(const unidb_param& params)
    {
        CHECK_UNIDB(real_db_, NAUT_DATABASE_E_DATABASE_IS_NULL);

        return real_db_->open(params);
    }

    int unidb::close()
    {
        CHECK_UNIDB(real_db_, NAUT_DATABASE_E_DATABASE_IS_NULL);

        return real_db_->close();
    }

    bool unidb::is_open()
    {
        CHECK_UNIDB(real_db_, false);

        return real_db_->is_open();
    }

    bool unidb::connected()
    {
        CHECK_UNIDB(real_db_, false);

        return real_db_->connected();
    }

    std::string unidb::host_info()
    {
        CHECK_UNIDB(real_db_, "");

        return real_db_->host_info();
    }

    std::string unidb::server_info()
    {
        CHECK_UNIDB(real_db_, "");

        return real_db_->server_info();
    }

    std::string unidb::charset()
    {
        CHECK_UNIDB(real_db_, "");

        return real_db_->charset();
    }

    std::string unidb::engine_type() const
    {
        return engine_type_;
    }

    const unidb_param& unidb::params() const
    {
        CHECK_UNIDB(real_db_, unidb_param_default);

        return real_db_->params();
    }

    /*********************************************************************/

    unidb_conn::unidb_conn(unidb_base* db)
        : unidb_conn_base(db)
        , real_db_conn_(NULL)
    {
        assert(db != NULL);

        std::string engine_type = db->engine_type();
        if (engine_type == "mysql") {
            real_db_conn_ = new db_mysql_conn(db);
        }
		else if (engine_type == "oracle") {
			real_db_conn_ = new db_oracle_conn(db);
		}
    }

    unidb_conn::~unidb_conn()
    {
        SAFE_RELEASE_OBJ(real_db_conn_);
    }

    bool unidb_conn::init_conn()
    {
        CHECK_UNIDB_CONN(real_db_conn_, false);

        return real_db_conn_->init_conn();
    }

    bool unidb_conn::release_conn()
    {
        CHECK_UNIDB_CONN(real_db_conn_, false);

        return real_db_conn_->release_conn();
    }

    bool unidb_conn::connected()
    {
        CHECK_UNIDB_CONN(real_db_conn_, false);

        return real_db_conn_->connected();
    }

    bool unidb_conn::begin_transaction()
    {
        CHECK_UNIDB_CONN(real_db_conn_, false);

        return real_db_conn_->begin_transaction();
    }

    bool unidb_conn::commit()
    {
        CHECK_UNIDB_CONN(real_db_conn_, false);

        return real_db_conn_->commit();
    }

    bool unidb_conn::rollback()
    {
        CHECK_UNIDB_CONN(real_db_conn_, false);

        return real_db_conn_->rollback();
    }

    bool unidb_conn::is_transaction()
    {
        CHECK_UNIDB_CONN(real_db_conn_, false);

        return real_db_conn_->is_transaction();
    }

    std::string unidb_conn::get_error()
    {
        CHECK_UNIDB_CONN(real_db_conn_, "");

        return real_db_conn_->get_error();
    }

    int unidb_conn::get_errno()
    {
        CHECK_UNIDB_CONN(real_db_conn_, NAUT_DATABASE_E_DATABASE_CONN_IS_NULL);

        return real_db_conn_->get_errno();
    }

    std::string unidb_conn::get_error_inner()
    {
        CHECK_UNIDB_CONN(real_db_conn_, "");

        return real_db_conn_->get_error_inner();
    }

    int unidb_conn::get_errno_inner()
    {
        CHECK_UNIDB_CONN(real_db_conn_, NAUT_DATABASE_E_DATABASE_CONN_IS_NULL);

        return real_db_conn_->get_errno_inner();
    }

    bool unidb_conn::execute(const char* sql)
    {
        CHECK_UNIDB_CONN(real_db_conn_, false);

        return real_db_conn_->execute(sql);
    }

    bool unidb_conn::query(const char* sql)
    {
        CHECK_UNIDB_CONN(real_db_conn_, false);

        return real_db_conn_->query(sql);
    }

    long unidb_conn::get_count() const
    {
        CHECK_UNIDB_CONN(real_db_conn_, 0);

        return real_db_conn_->get_count();
    }

    long unidb_conn::fields_count() const
    {
        CHECK_UNIDB_CONN(real_db_conn_, 0);

        return real_db_conn_->fields_count();
    }

    void unidb_conn::free_result()
    {
        CHECK_UNIDB_CONN_VOID(real_db_conn_);

        real_db_conn_->free_result();
    }

    std::vector<std::string> unidb_conn::get_fields()
    {
        if (real_db_conn_ == NULL) {
            std::vector<std::string> ret;
            return ret;
        }

        return real_db_conn_->get_fields();
    }

    bool unidb_conn::first()
    {
        CHECK_UNIDB_CONN(real_db_conn_, false);

        return real_db_conn_->first();
    }

    bool unidb_conn::fetch_row()
    {
        CHECK_UNIDB_CONN(real_db_conn_, false);

        return real_db_conn_->fetch_row();
    }

    int unidb_conn::affected_rows()
    {
        CHECK_UNIDB_CONN(real_db_conn_, false);

        return real_db_conn_->affected_rows();
    }

    bool unidb_conn::is_null(int id)
    {
        CHECK_UNIDB_CONN(real_db_conn_, false);

        return real_db_conn_->is_null(id);
    }

    bool unidb_conn::is_null(const std::string& field_name)
    {
        CHECK_UNIDB_CONN(real_db_conn_, false);

        return real_db_conn_->is_null(field_name);
    }

    bool unidb_conn::is_null(const char* field_name)
    {
        CHECK_UNIDB_CONN(real_db_conn_, false);

        return real_db_conn_->is_null(field_name);
    }

    const char* unidb_conn::get_string(int id)
    {
        CHECK_UNIDB_CONN(real_db_conn_, "");

        return real_db_conn_->get_string(id);
    }

    const char* unidb_conn::get_string(const std::string& field_name)
    {
        CHECK_UNIDB_CONN(real_db_conn_, "");

        return real_db_conn_->get_string(field_name);
    }

    const char* unidb_conn::get_string(const char* field_name)
    {
        CHECK_UNIDB_CONN(real_db_conn_, "");

        return real_db_conn_->get_string(field_name);
    }

    double unidb_conn::get_double(int id)
    {
        CHECK_UNIDB_CONN(real_db_conn_, 0);

        return real_db_conn_->get_double(id);
    }

    double unidb_conn::get_double(const std::string& field_name)
    {
        CHECK_UNIDB_CONN(real_db_conn_, 0);

        return real_db_conn_->get_double(field_name);
    }

    double unidb_conn::get_double(const char* field_name)
    {
        CHECK_UNIDB_CONN(real_db_conn_, 0);

        return real_db_conn_->get_double(field_name);
    }

    long unidb_conn::get_long(int id)
    {
        CHECK_UNIDB_CONN(real_db_conn_, 0);

        return real_db_conn_->get_long(id);
    }

    long unidb_conn::get_long(const std::string& field_name)
    {
        CHECK_UNIDB_CONN(real_db_conn_, 0);

        return real_db_conn_->get_long(field_name);
    }

    long unidb_conn::get_long(const char* field_name)
    {
        CHECK_UNIDB_CONN(real_db_conn_, 0);

        return real_db_conn_->get_long(field_name);
    }

    base::int64 unidb_conn::get_int64(int id)
    {
        CHECK_UNIDB_CONN(real_db_conn_, 0);

        return real_db_conn_->get_int64(id);
    }

    base::int64 unidb_conn::get_int64(const std::string& field_name)
    {
        CHECK_UNIDB_CONN(real_db_conn_, 0);

        return real_db_conn_->get_int64(field_name);
    }

    base::int64 unidb_conn::get_int64(const char* field_name)
    {
        CHECK_UNIDB_CONN(real_db_conn_, 0);

        return real_db_conn_->get_int64(field_name);
    }
}
