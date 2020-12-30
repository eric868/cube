#ifndef MYSQl_WRAPPER_H_ 
#define MYSQl_WRAPPER_H_ 

#include <stdio.h>
#include <string.h>
#include <iostream> 
#include <string> 
#include <vector> 
#include <string> 
#include <memory>
#include <mutex>
#include <functional>

#include "utility/utility.h"
#include "mysql-5.7.22/include/mysql.h" 

namespace mysql_wrapper {

    struct ConnectParam
    {
        std::string host;
        int port;
        std::string user;
        std::string password;
        std::string database_name;

        ConnectParam()
            : host("127.0.0.1")
            , port(3306)
            , user("risk")
            , password("risk")
            , database_name("test")
        {
        }
    };

    class COMMON_API MysqlWrapper
    {
    public:
        MysqlWrapper();
        ~MysqlWrapper();

        bool connet(const ConnectParam& param);
        void close();

        bool create_database(std::string& dbname);
        bool create_table(const std::string& query);

        bool execute_sql(std::string strSql);
        bool fetch_data(std::string queryStr, 
            std::vector<std::vector<std::string> >& data);
        int affected_rows();

        void run_failed();
        void get_last_error(int &nErrorCode, std::string &errorMsg);

    private:
        ConnectParam param_;

        int error_num;          //错误代号 
        const char* error_info; //错误提示 

        MYSQL mysql_instance;   //MySQL对象，必备的一个数据结构 
        MYSQL_RES *result;      //用于存放结果 建议用char* 数组将此结果转存 
    };

    template <class T>
    class ObjectPool
    {
    public:
        using DeleterType = std::function<void(T*)>;

        void Init(const ConnectParam &param, int size)
        {
            param_ = param;
            std::lock_guard<std::mutex> locker(mutex_);
            for (int i = 0; i < size; ++i) {
                add();
            }
        }

        void add()
        {
            T* ptr = new T();
            ptr->connet(param_);
            pool_.push_back(std::unique_ptr<T>(ptr));
        }

        std::unique_ptr<T, DeleterType> get()
        {
            std::lock_guard<std::mutex> locker(mutex_);
            if (pool_.empty()) {
                cout << "no more object" << endl;
                //return nullptr;
                add();
            }

            //every time add custom deleter for default unique_ptr
            std::unique_ptr<T, DeleterType> ptr(pool_.back().release(), 
                [this](T* t) { pool_.push_back(std::unique_ptr<T>(t)); });

            pool_.pop_back();
            return std::move(ptr);
        }

        bool empty()
        {
            std::lock_guard<std::mutex> locker(mutex_);
            return pool_.empty();
        }

        size_t size()
        {
            std::lock_guard<std::mutex> locker(mutex_);
            return pool_.size();
        }

    private:
        ConnectParam param_;
        std::mutex mutex_;
        std::vector<std::unique_ptr<T>> pool_;
    };

}

#endif

