
#include "mysql_wrapper.h"
   
namespace mysql_wrapper {

    MysqlWrapper::MysqlWrapper()
        : error_num(0)
        , error_info("ok")
    {
        mysql_library_init(0, NULL, NULL);
        mysql_init(&mysql_instance);
        mysql_options(&mysql_instance, MYSQL_SET_CHARSET_NAME, "gbk");
    }

    MysqlWrapper::~MysqlWrapper()
    {
        close();
    }

    //连接MySQL
    bool MysqlWrapper::connet(const ConnectParam& param)
    {
        param_ = param;
        if (mysql_real_connect(&mysql_instance, param_.host.c_str(), 
            param_.user.c_str(), param_.password.c_str(),
            param_.database_name.c_str(), param_.port, 0, 
            CLIENT_MULTI_STATEMENTS) != NULL) {

            my_bool status = true;
            mysql_options(&mysql_instance, MYSQL_OPT_RECONNECT, &status);

            return true;
        } 
        run_failed();
        return false;
    }

    //判断数据库是否存在，不存在则创建数据库，并打开 
    bool MysqlWrapper::create_database(std::string& dbname)
    {
        std::string query_str = "create database if not exists ";
        query_str += dbname;
        if (0 == mysql_query(&mysql_instance, query_str.c_str())) {
            query_str = "use ";
            query_str += dbname;
            if (0 == mysql_query(&mysql_instance, query_str.c_str())) {
                return true;
            }
        }
        run_failed();
        return false;
    }

    //判断数据库中是否存在相应表，不存在则创建表 
    bool MysqlWrapper::create_table(const std::string& sql)
    {
        if (0 == mysql_query(&mysql_instance, sql.c_str())) {
            return true;
        }
        run_failed();
        return false;
    }

    //写入数据 
    bool MysqlWrapper::execute_sql(std::string sql)
    {
        if (0 == mysql_query(&mysql_instance, sql.c_str())) {
            return true;
        } 
        run_failed();
        return false;
    }

    //读取数据 
    bool MysqlWrapper::fetch_data(std::string query,
        std::vector<std::vector<std::string> >& data)
    {
        if (0 != mysql_query(&mysql_instance, query.c_str())) {
            run_failed();
            return false;
        }

        result = mysql_store_result(&mysql_instance);
        int row = mysql_num_rows(result);
        int field = mysql_num_fields(result);

        MYSQL_ROW line = NULL;
        line = mysql_fetch_row(result);

        int j = 0;
        std::string temp;
        while (NULL != line) {
            std::vector<std::string> linedata;
            for (int i = 0; i < field;i++) {
                if (line[i]) {
                    temp = line[i];
                    linedata.push_back(temp);
                } else {
                    temp = "";
                    linedata.push_back(temp);
                }
            }
            line = mysql_fetch_row(result);
            data.push_back(linedata);
        }
        mysql_free_result(result);

        return true;
    }

    int MysqlWrapper::affected_rows()
    {
        return mysql_affected_rows(&mysql_instance);
    }

    //错误信息 
    void MysqlWrapper::run_failed()
    {
        error_num = mysql_errno(&mysql_instance);
        error_info = mysql_error(&mysql_instance);

        printf("ERROR:操作数据库失败!error_num:%d error_info:%s\n", 
            error_num, error_info);
    }

    void MysqlWrapper::get_last_error(int &error_code, std::string &error_msg)
    {
        error_code = error_num;
        if (error_num) {
            error_msg = std::string(error_info);
        }
    }

    //断开连接 
    void MysqlWrapper::close()
    {
        mysql_close(&mysql_instance);
    }

}
