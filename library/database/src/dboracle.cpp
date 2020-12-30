/*****************************************************************************
Nautilus Database Copyright (c) 2017. All Rights Reserved.

FileName: dboracle.cpp
Version: 1.0
Date: 2017.3.31

History:
shengkaishan     2017.3.31   1.0     Create
******************************************************************************/

#include "dboracle.h"
#include "common.h"
#include "base/base.h"
#include "base/trace.h"
#include <stdio.h>

namespace database
{

/*********************************************************************/

const char* oracle_null_string = "";

void err_handler(OCI_Error *err)
{
    printf("Error_code=%d, Error_msg=%s\n", OCI_ErrorGetOCICode(err),
        OCI_ErrorGetString(err));
}

db_oracle::db_oracle()
    : connection_(NULL)
    , statement_(NULL)
    , is_open_(false)
    , conn_count_(0)
    , last_error_(NAUT_DATABASE_S_OK)
{
    OCI_Initialize(err_handler, NULL, OCI_ENV_DEFAULT);
}

db_oracle::~db_oracle()
{
    close();
    OCI_Cleanup();
}

int db_oracle::open(const unidb_param& params)
{
    if (is_open_) {
        return NAUT_DATABASE_S_OK;
    }

    params_= params;

    last_error_ = NAUT_DATABASE_S_OK;

    if (params_.recreate_database_if_exists) {
        last_error_ = recreate_database_if_exists(params_.database_name.c_str());
        CHECK_LABEL(last_error_, end);
    }

    if (params_.create_database_if_not_exists) {
        last_error_ = create_database_if_not_exists(params_.database_name.c_str());
        CHECK_LABEL(last_error_, end);
    }  
    connection_ = OCI_ConnectionCreate(params_.host.c_str(), params_.user.c_str(), 
        params_.password.c_str(), OCI_SESSION_DEFAULT);

end:
    if (BSUCCEEDED(last_error_)) {
        is_open_ = true;
    }
    return last_error_;
}

int db_oracle::close()
{
    if (conn_count_ > 0) {
        TRACE_ERROR("database", NAUT_DATABASE_E_ALL_CONN_SHOULD_RELEASE,
                "all connections should be released before close database.");
        return NAUT_DATABASE_E_ALL_CONN_SHOULD_RELEASE;
    }

    if (is_open_) {
        OCI_ConnectionFree(connection_);
        is_open_ = false;
    }

    return NAUT_DATABASE_S_OK;
}

bool db_oracle::is_open()
{
    return is_open_;
}

bool db_oracle::connected()
{
    //int ret = mysql_ping(&mysql_);
    //if (ret != 0) {
    //    TRACE_ERROR("database", NAUT_DATABASE_E_PING_MYSQL_FAILED,
    //        "ping mysql failed, error:%d,%s", ret, mysql_error(&mysql_));
    //    return false;
    //}
    return true;
}

std::string db_oracle::host_info()
{
    if (!is_open_) {
        TRACE_WARNING("database", "the database has not been initialized");
        return "";
    }
    //return mysql_get_host_info(&ORACLE_);
    return "";
}

std::string db_oracle::server_info()
{
    if (!is_open_) {
        TRACE_WARNING("database", "the database has not been initialized");
        return "";
    }
    //return mysql_get_server_info(&ORACLE_);
    return "";
}

std::string db_oracle::charset()
{
    if (!is_open_) {
        TRACE_WARNING("database", "the database has not been initialized");
        return "";
    }
    //return mysql_character_set_name(&ORACLE_);
    return "";
}

std::string db_oracle::engine_type() const
{
    return "oracle";
}

const unidb_param& db_oracle::params() const
{
    return params_;
}

int db_oracle::create_database_if_not_exists(const char* database_name)
{
    assert(database_name != NULL && strlen(database_name) > 0);

    return last_error_;
}

int db_oracle::recreate_database_if_exists(const char* database_name)
{
    assert(database_name != NULL && strlen(database_name) > 0);

    return last_error_;
}

void db_oracle::add_connection(db_oracle_conn* conn)
{
    base_fetch_and_inc(&conn_count_);
}

void db_oracle::release_connection(db_oracle_conn* conn)
{
    base_fetch_and_dec(&conn_count_);
}

/*********************************************************************/

db_oracle_conn::db_oracle_conn(unidb_base* db)
    : unidb_conn_base(db)
    , db_(db)
    , connection_(NULL)
    , statement_(NULL)
    , result_(NULL)
    , is_transaction_(false)
    , num_cols_(0)
    , initialized_(false)
    , last_error_(NAUT_DATABASE_S_OK)
    , last_error_inner_(0)
{
    assert(db != NULL);
}

db_oracle_conn::~db_oracle_conn()
{
    release_conn();
}

bool db_oracle_conn::init_conn()
{
    if (initialized_) {
        return true;
    }

    assert(db_ != NULL);

    LABEL_SCOPE_START;

    last_error_ = NAUT_DATABASE_S_OK;
    last_error_inner_ = 0;
    slast_error_inner_ = "";

    last_error_ = real_connect();
    CHECK_LABEL(last_error_, end);

    LABEL_SCOPE_END;

end:
    if (BSUCCEEDED(last_error_)) {
        initialized_ = true;
    }
    return (last_error_ == NAUT_DATABASE_S_OK);
}

bool db_oracle_conn::release_conn()
{
    if (initialized_) {
        commit();
        free_result();
        if (statement_ != NULL) {
            OCI_StatementFree(statement_);
            statement_ = NULL;
        }
        if (connection_ != NULL) {
            OCI_ConnectionFree(connection_);
            connection_ = NULL;
        }
        initialized_ = false;
    }
    return true;
}

bool db_oracle_conn::connected()
{

    if (!initialized_) {
        TRACE_WARNING("database", "oracle connection is not initialized or initialize failed");
        return false;
    }

    //int ret = mysql_ping(&mysql_);
    //if (ret) {
    //    TRACE_ERROR("database", NAUT_DATABASE_E_PING_MYSQL_FAILED,
    //        "ping mysql failed, error:%d,%s", ret, mysql_error(&mysql_));
    //    last_error_ = NAUT_DATABASE_E_PING_MYSQL_FAILED;
    //    return false;
    //}

    return true;
}

bool db_oracle_conn::reconnect()
{
    last_error_ = real_connect();

    return BSUCCEEDED(last_error_);
}

int db_oracle_conn::real_connect()
{
    int ret = TRUE;

    unidb_param params = db_->params();

    connection_ = OCI_ConnectionCreate(params.host.c_str(), params.user.c_str(),
        params.password.c_str(), OCI_SESSION_DEFAULT);
    statement_ = OCI_StatementCreate(connection_);

    ret = OCI_SetAutoCommit(connection_, TRUE);
    if (ret != TRUE) {
        TRACE_ERROR("database", NAUT_DATABASE_E_ORACLE_SET_AUTOCOMMIT_FAILED,
            "oracle set auto commit failed, error:%d,%s", 
            OCI_ErrorGetOCICode(OCI_GetLastError()), 
            OCI_ErrorGetString(OCI_GetLastError()));
        ASSIGN_AND_CHECK_LABEL(ret, NAUT_DATABASE_E_ORACLE_SET_AUTOCOMMIT_FAILED, end);
    }

end:
    return (ret == TRUE) ? 0 : 1;
}

bool db_oracle_conn::begin_transaction()
{
    if (!initialized_) {
        TRACE_WARNING("database", "oracle connection is not initialized or initialize failed");
        return false;
    }

    int ret = 0;
#if 0
    OCI_Transaction *trans = OCI_TransactionCreate(connection_, 
        0, OCI_TRS_SERIALIZABLE, NULL);
    if (trans == NULL) {
        if (!connected() && reconnect()) {
            OCI_Transaction *trans = OCI_TransactionCreate(connection_,
                0, OCI_TRS_SERIALIZABLE, NULL);
        }
    }
    if (trans == NULL) {
        TRACE_ERROR("database", NAUT_DATABASE_E_ORACLE_BEGIN_TRANSACTION_FAILED,
                "begin transaction failed, error: %d,%s",
                OCI_ErrorGetOCICode(OCI_GetLastError()),
                OCI_ErrorGetString(OCI_GetLastError()));
        last_error_ = NAUT_DATABASE_E_ORACLE_BEGIN_TRANSACTION_FAILED;
        ret = -1;
    }
    else
#endif
    {
        //commit();
        //OCI_TransactionStart(trans);
        set_auto_commit(false);
        is_transaction_ = true;
    }

    return (ret == 0);
}

bool db_oracle_conn::commit()
{
    if (!initialized_) {
        TRACE_WARNING("database", "oracle connection is not initialized or initialize failed");
        return false;
    }

    int ret = OCI_Commit(connection_);
    if (ret != TRUE) {
        TRACE_ERROR("database", NAUT_DATABASE_E_ORACLE_COMMIT_FAILED,
                "oracle commit transaction failed, error:%d,%s", 
                OCI_ErrorGetOCICode(OCI_GetLastError()),
                OCI_ErrorGetString(OCI_GetLastError()));
        last_error_ = NAUT_DATABASE_E_ORACLE_COMMIT_FAILED;
    }
    
    set_auto_commit(true);
    is_transaction_ = false;
    return (ret == TRUE);
}

bool db_oracle_conn::rollback()
{
    if (!initialized_) {
        TRACE_WARNING("database", "oracle connection is not initialized or initialize failed");
        return false;
    }

    int ret = OCI_Rollback(connection_);
    if (ret != TRUE) {
        TRACE_ERROR("database", NAUT_DATABASE_E_ORACLE_ROLLBACK_FAILED,
                "oracle rollback transaction failed, error:%d,%s", 
                OCI_ErrorGetOCICode(OCI_GetLastError()),
                OCI_ErrorGetString(OCI_GetLastError()));
        last_error_ = NAUT_DATABASE_E_ORACLE_ROLLBACK_FAILED;
    }
    set_auto_commit(true);
    is_transaction_ = false;
    return (ret == TRUE);
}

bool db_oracle_conn::set_auto_commit(bool auto_commit)
{
    if (!initialized_) {
        TRACE_WARNING("database", "oracle connection is not initialized or initialize failed");
        return false;
    }

    int ret = OCI_SetAutoCommit(connection_, auto_commit);
    if (ret != TRUE) {
        TRACE_ERROR("database", NAUT_DATABASE_E_ORACLE_SET_AUTOCOMMIT_FAILED,
            "oracle set auto commit failed, error:%d,%s",
            OCI_ErrorGetOCICode(OCI_GetLastError()),
            OCI_ErrorGetString(OCI_GetLastError()));
        last_error_ = NAUT_DATABASE_E_ORACLE_SET_AUTOCOMMIT_FAILED;
        return false;
    }
    return true;
}

bool db_oracle_conn::is_transaction()
{
    return is_transaction_;
}

std::string db_oracle_conn::get_error()
{
    switch(last_error_) {
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
    case NAUT_DATABASE_E_INIT_ORACLE_FAILED:
        return "NAUT_DATABASE_E_INIT_ORACLE_FAILED";
    case NAUT_DATABASE_E_CONNECT_TO_ORACLE_FAILED:
        return "NAUT_DATABASE_E_CONNECT_TO_ORACLE_FAILED";
    case NAUT_DATABASE_E_PING_ORACLE_FAILED:
        return "NAUT_DATABASE_E_PING_ORACLE_FAILED";
    case NAUT_DATABASE_E_ORACLE_SET_AUTOCOMMIT_FAILED:
        return "NAUT_DATABASE_E_ORACLE_SET_AUTOCOMMIT_FAILED";
    case NAUT_DATABASE_E_ORACLE_COMMIT_FAILED:
        return "NAUT_DATABASE_E_ORACLE_COMMIT_FAILED";
    case NAUT_DATABASE_E_ORACLE_ROLLBACK_FAILED:
        return "NAUT_DATABASE_E_ORACLE_ROLLBACK_FAILED";
    case NAUT_DATABASE_E_ORACLE_EXECUTE_SQL_FAILED:
        return "NAUT_DATABASE_E_ORACLE_EXECUTE_SQL_FAILED";
    case NAUT_DATABASE_E_ORACLE_EXECUTE_QUERY_FAILED:
        return "NAUT_DATABASE_E_ORACLE_EXECUTE_QUERY_FAILED";
    case NAUT_DATABASE_E_ORACLE_CONN_UNINITIALIZED:
        return "NAUT_DATABASE_E_ORACLE_CONN_UNINITIALIZED";
    default:
        return "unknown error code";
    }
}

int db_oracle_conn::get_errno()
{
    return last_error_;
}

std::string db_oracle_conn::get_error_inner()
{
    return slast_error_inner_;
}

int db_oracle_conn::get_errno_inner()
{
    return last_error_inner_;
}

bool db_oracle_conn::execute(const char* sql)
{
    assert(sql != NULL && strlen(sql) > 0);

    if (!initialized_) {
        TRACE_WARNING("database", "oracle connection is not initialized or initialize failed");
        return false;
    }

    free_result();

    //if (!is_transaction_) {
    //    set_auto_commit(true);
    //}

    int ret = OCI_ExecuteStmt(statement_, sql);
    if (ret != TRUE) {
        if (!connected() && reconnect()) {
            ret = OCI_ExecuteStmt(statement_, sql);
        }
    }
    if (ret != TRUE) {
        TRACE_ERROR("database", NAUT_DATABASE_E_ORACLE_EXECUTE_SQL_FAILED,
            "execute sql '%s' failed, error:%d,%s", sql,
            OCI_ErrorGetOCICode(OCI_GetLastError()),
            OCI_ErrorGetString(OCI_GetLastError()));
        last_error_ = NAUT_DATABASE_E_ORACLE_EXECUTE_SQL_FAILED;
    }
    
    //printf("affected_rows = %d\n", OCI_GetAffectedRows(statement_));
    //OCI_Commit(connection_);

    return (ret == TRUE);
}

bool db_oracle_conn::query(const char* sql)
{
    assert(sql != NULL && strlen(sql) > 0);

    if (!initialized_) {
        TRACE_WARNING("database", "oracle connection is not initialized or initialize failed");
        return false;
    }

    free_result();

    int ret = OCI_ExecuteStmt(statement_, sql);
    if (ret != TRUE) {
        if (!connected() && reconnect()) {
            ret = OCI_ExecuteStmt(statement_, sql);
        }
    }
    if (ret != TRUE) {
        TRACE_ERROR("database", NAUT_DATABASE_E_ORACLE_EXECUTE_QUERY_FAILED,
            "execute sql '%s' failed, error:%d,%s", sql,
            OCI_ErrorGetOCICode(OCI_GetLastError()),
            OCI_ErrorGetString(OCI_GetLastError()));
        last_error_ = NAUT_DATABASE_E_ORACLE_EXECUTE_QUERY_FAILED;
        return false;
    } 
    else {
        result_ = OCI_GetResultset(statement_);
        if (result_ != NULL) {
            num_cols_ = OCI_GetColumnCount(result_);
            for (int i = 1; i <= num_cols_; i++) {
                OCI_Column *field = OCI_GetColumn(result_, i);
                map_fields_[OCI_ColumnGetName(field)] = i;
            }
        }
    }

    return true;
}

long db_oracle_conn::get_count() const
{
    int row_count = 0;
#if 0
    if (result_ != NULL) {
        while (OCI_FetchNext(result_));
        row_count = OCI_GetRowCount(result_);

        //OCI_FetchFirst(result_);
    }
#endif
    return row_count;
}

long db_oracle_conn::fields_count() const
{
    return num_cols_;
}

void db_oracle_conn::free_result()
{
    if (result_ != NULL) {
        OCI_ReleaseResultsets(statement_);
        result_ = NULL;
        map_fields_.clear();
    }
    num_cols_ = 0;
}

std::vector<std::string> db_oracle_conn::get_fields()
{
    std::vector<std::string> vfields;
    VBASE_HASH_MAP<const char*, int, string_hash, string_compare>::iterator it = map_fields_.begin();
    while (it != map_fields_.end()) {
        vfields.push_back(it->first);
        it++;
    }
    return vfields;
}


bool db_oracle_conn::first()
{
    if (result_ != NULL) {
        OCI_FetchFirst(result_);
    }
    return false;
}

bool db_oracle_conn::fetch_row()
{
    if (OCI_FetchNext(result_)) {
        return true;
    }

    return false;
}

int db_oracle_conn::affected_rows()
{
    return OCI_GetAffectedRows(statement_);
}

bool db_oracle_conn::is_null(int id)
{
    assert(id >= 0 && id < num_cols_);

    if (result_ != NULL) {
        return OCI_IsNull(result_, id) ? true : false;
    }
    return true;
}

bool db_oracle_conn::is_null(const std::string& field_name)
{
    return is_null(field_name.c_str());
}

bool db_oracle_conn::is_null(const char* field_name)
{
    //int index = map_fields_[field_name];
    //if (index >= 1) {
    //    return is_null(index);
    //}
    //return true;
    if (result_ != NULL) {
        return OCI_IsNull2(result_, field_name) ? true : false;
    }
    return true;
}

const char* db_oracle_conn::get_string(int id)
{
    assert(id > 0 && id <= num_cols_);

    const char* ret = NULL;
    if (result_ != NULL) {
        ret = OCI_GetString(result_, id);
    }

    return ret == NULL ? oracle_null_string : ret;
}

const char* db_oracle_conn::get_string(const std::string& field_name)
{
    return get_string(field_name.c_str());
}

const char* db_oracle_conn::get_string(const char* field_name)
{
    //int index = map_fields_[field_name];
    //if (index >= 1) {
    //    return get_string(index);
    //}
    //return oracle_null_string;
    const char* ret = NULL;
    if (result_ != NULL) {
        ret = OCI_GetString2(result_, field_name);
    }

    return ret == NULL ? oracle_null_string : ret;
}

double db_oracle_conn::get_double(int id)
{
    assert(id > 0 && id <= num_cols_);

    if (result_ != NULL && !OCI_IsNull(result_, id)) {
        //return atof(OCI_GetString(result_, id));
        return OCI_GetDouble(result_, id);
    }
    return 0;
}

double db_oracle_conn::get_double(const std::string& field_name)
{
    return get_double(field_name.c_str());
}

double db_oracle_conn::get_double(const char* field_name)
{
    //int index = map_fields_[field_name];
    //if (index >= 1) {
    //    return get_double(index);
    //}
    //return 0;
    if (result_ != NULL && !OCI_IsNull2(result_, field_name)) {
        return OCI_GetDouble2(result_, field_name);
    }
    return 0;
}

long db_oracle_conn::get_long(int id)
{
    assert(id > 0 && id <= num_cols_);

    if (result_ != NULL && !OCI_IsNull(result_, id)) {
        return atol(OCI_GetString(result_, id));
    }
    return 0;
}

long db_oracle_conn::get_long(const std::string& field_name)
{
    return get_long(field_name.c_str());
}

long db_oracle_conn::get_long(const char* field_name)
{
    //int index = map_fields_[field_name];
    //if (index >= 1) {
    //    return get_long(index);
    //}
    //return 0;
    if (result_ != NULL && !OCI_IsNull2(result_, field_name)) {
        return atol(OCI_GetString2(result_, field_name));
    }
    return 0;
}

base::int64 db_oracle_conn::get_int64(int id)
{
    assert(id > 0 && id <= num_cols_);

    if (result_ != NULL && !OCI_IsNull(result_, id)) {
        return OCI_GetBigInt(result_, id);
    }
    return 0;
}

base::int64 db_oracle_conn::get_int64(const std::string& field_name)
{
    return get_int64(field_name.c_str());
}

base::int64 db_oracle_conn::get_int64(const char* field_name)
{
    //int index = map_fields_[field_name];
    //if (index >= 1) {
    //    return get_int64(index);
    //}
    //return 0;

    if (result_ != NULL && !OCI_IsNull2(result_, field_name)) {
        return OCI_GetBigInt2(result_, field_name);
    }
    return 0;
}

}
