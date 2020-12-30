/*****************************************************************************
 Nautilus Database Copyright (c) 2015. All Rights Reserved.

 FileName: unidb.h
 Version: 1.0
 Date: 2015.9.5

 History:
 database     2015.9.5   1.0     Create
 ******************************************************************************/

#ifndef __NAUT_DATABASE_UNIDB_H__
#define __NAUT_DATABASE_UNIDB_H__

#include "base/base.h"
#include <string>
#include <vector>
namespace database
{
    struct unidb_param
    {
        /* host of the database */
        std::string host;

        /* port of the database host */
        int port;

        /* database user */
        std::string user;

        /* database password */
        std::string password;

        /* target database name */
        std::string database_name;

        /* charset of the database */
        std::string charset;

        /* create database if not exists */
        bool create_database_if_not_exists;

        /* recreate database if exists */
        bool recreate_database_if_exists;

        unidb_param()
            : host("127.0.0.1")
            , port(3306)
            , user("root")
            , password("000000")
            , database_name("test")
            , charset("utf8")
            , create_database_if_not_exists(true)
            , recreate_database_if_exists(false)
        {
        }
    };

    static const unidb_param unidb_param_default;

    class unidb_base
    {
    public:
        virtual ~unidb_base() {}

    public:
        virtual int open(const unidb_param& params) = 0;
        virtual int close() = 0;
        virtual bool is_open() = 0;
        virtual bool connected() = 0;

        virtual std::string host_info() = 0;
        virtual std::string server_info() = 0;
        virtual std::string charset() = 0;
        virtual std::string engine_type() const = 0;

        virtual const unidb_param& params() const = 0;
    };

    /*********************************************************************/

    class COMMON_API unidb
        : public unidb_base
    {
    public:
        unidb(const std::string& engine_type = "mysql");
        virtual ~unidb();

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

    private:
        unidb_base* real_db_;
        std::string engine_type_;
    };

    /*********************************************************************/

    class unidb_conn_base
    {
    public:
        unidb_conn_base(unidb_base* db) {}
        virtual ~unidb_conn_base() {}

    public:
        virtual bool init_conn() = 0;
        virtual bool release_conn() = 0;

        virtual bool connected() = 0;
        virtual bool begin_transaction() = 0;
        virtual bool commit() = 0;
        virtual bool rollback() = 0;
        virtual bool is_transaction() = 0;

        virtual std::string get_error() = 0;
        virtual int get_errno() = 0;
        virtual std::string get_error_inner() = 0;
        virtual int get_errno_inner() = 0;

    public:
        virtual bool execute(const char* sql) = 0;
        virtual bool query(const char* sql) = 0;
        virtual long get_count() const = 0;
        virtual long fields_count() const = 0;
        virtual void free_result() = 0;
        virtual std::vector<std::string> get_fields() = 0;

        virtual bool first() = 0;
        virtual bool fetch_row() = 0;
        virtual int affected_rows() = 0;

        virtual bool is_null(int id) = 0;
        virtual bool is_null(const std::string& field_name) = 0;
        virtual bool is_null(const char* field_name) = 0;
        virtual const char* get_string(int id) = 0;
        virtual const char* get_string(const std::string& field_name) = 0;
        virtual const char* get_string(const char* field_name) = 0;
        virtual double get_double(int id) = 0;
        virtual double get_double(const std::string& field_name) = 0;
        virtual double get_double(const char* field_name) = 0;
        virtual long get_long(int id) = 0;
        virtual long get_long(const std::string& field_name) = 0;
        virtual long get_long(const char* field_name) = 0;
        virtual base::int64 get_int64(int id) = 0;
        virtual base::int64 get_int64(const std::string& field_name) = 0;
        virtual base::int64 get_int64(const char* field_name) = 0;
    };

    /*********************************************************************/

    class COMMON_API unidb_conn
        : public unidb_conn_base
    {
    public:
        unidb_conn(unidb_base* db);
        virtual ~unidb_conn();

    public:
        virtual bool init_conn();
        virtual bool release_conn();

        virtual bool connected();
        virtual bool begin_transaction();
        virtual bool commit();
        virtual bool rollback();
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

    private:
        unidb_conn_base* real_db_conn_;
    };

}

#endif
