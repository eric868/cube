/*****************************************************************************
 Nautilus Database Copyright (c) 2017. All Rights Reserved.

 FileName: dboracle.h
 Version: 1.0
 Date: 2017.3.31

 History:
 shengkaishan     2017.3.31   1.0     Create
 ******************************************************************************/

#ifndef __NAUT_DATABASE_ORACLE_H__
#define __NAUT_DATABASE_ORACLE_H__

#ifdef _WIN32   
#include "winsock.h"
#endif 

#include <map>
#include "base/base.h"
#include "unidb.h"
#include "ocilib-master/include/ocilib.h"

namespace database
{

/*********************************************************************/

class db_oracle_conn;
class db_oracle
    : public unidb_base
{
    friend class db_oracle_conn;

public:
    db_oracle();
    virtual ~db_oracle();

public:
    virtual int open(const unidb_param& params);
    virtual int close();
    virtual bool is_open();
    virtual bool connected();

    virtual std::string host_info();
    virtual std::string server_info();
    virtual std::string charset();
    virtual std::string engine_type() const;

    virtual const unidb_param& params() const;

protected:
    int create_database_if_not_exists(const char* database_name);
    int recreate_database_if_exists(const char* database_name);

protected:
    void add_connection(db_oracle_conn* conn);
    void release_connection(db_oracle_conn* conn);

private:
    unidb_param params_;
    OCI_Connection *connection_;
    OCI_Statement  *statement_;
    bool is_open_;
    volatile long conn_count_;
    int last_error_;
};

/*********************************************************************/

class db_oracle_conn
    : public unidb_conn_base
{
public:
    db_oracle_conn(unidb_base* db);
    virtual ~db_oracle_conn();

public:
    virtual bool init_conn();
    virtual bool release_conn();

    virtual bool connected();
    virtual bool begin_transaction();
    virtual bool commit();
    virtual bool rollback();
    virtual bool set_auto_commit(bool auto_commit);
    virtual bool is_transaction();

    virtual std::string get_error();
    virtual int get_errno();
    virtual std::string get_error_inner();
    virtual int get_errno_inner();

public:
    virtual bool execute(const char* sql);
    virtual bool query(const char* sql);
    virtual long get_count() const;
    virtual long fields_count() const;
    virtual void free_result();
    virtual std::vector<std::string> get_fields();

    virtual bool first();
    virtual bool fetch_row();
    virtual int affected_rows();

    virtual bool is_null(int id);
    virtual bool is_null(const std::string& field_name);
    virtual bool is_null(const char* field_name);
    virtual const char* get_string(int id);
    virtual const char* get_string(const std::string& field_name);
    virtual const char* get_string(const char* field_name);
    virtual double get_double(int id);
    virtual double get_double(const std::string& field_name);
    virtual double get_double(const char* field_name);
    virtual long get_long(int id);
    virtual long get_long(const std::string& field_name);
    virtual long get_long(const char* field_name);
    virtual base::int64 get_int64(int id);
    virtual base::int64 get_int64(const std::string& field_name);
    virtual base::int64 get_int64(const char* field_name);

protected:
    bool reconnect();
    int real_connect();

private:
    unidb_base* db_;
    OCI_Connection *connection_;
    OCI_Statement  *statement_;
    OCI_Resultset  *result_;
    VBASE_HASH_MAP<const char*, int, string_hash, string_compare> map_fields_;
    //VBASE_HASH_MAP<std::string, int> map_fields_;
    bool is_transaction_;
    int num_cols_;
    bool initialized_;
    int last_error_;
    int last_error_inner_;
    std::string slast_error_inner_;
};

}

#endif
