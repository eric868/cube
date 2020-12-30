#include <iostream>
#include <string>
#include "mysql_api/mysql_wrapper.h"

using namespace std;

int main(int argc,char* argv[])
{
    mysql_wrapper::ConnectParam param;
    param.host = "192.168.4.44";
    param.port = 3306;
    param.user = "risk";
    param.password = "risk";
    param.database_name = "future_risk";
    mysql_wrapper::ObjectPool<mysql_wrapper::MysqlWrapper> mysql_pool;
    mysql_pool.Init(param, 3);

    auto t = mysql_pool.get();
    {
        auto t2 = mysql_pool.get();
    }
    

    auto t3 = mysql_pool.get();
    auto t4 = mysql_pool.get();
    std::string query_str = "select instrumentid, exchangeid from t_instrument";
    std::vector<std::vector<std::string> > result;
    t4->fetch_data(query_str, result);
    for (auto it = result.begin(); it != result.end(); ++it) {
        cout << (*it)[0]  << " " << (*it)[1] << endl;
    }
    getchar();
	return 0;
}