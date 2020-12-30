/*****************************************************************************
 Nautilus Database Copyright (c) 2015. All Rights Reserved.

 FileName: test_unidb.cpp
 Version: 1.0
 Date: 2015.9.6

 History:
 jeffguo     2015.9.6   1.0     Create
 ******************************************************************************/

#include "test_unidb.h"
#include <string>
#include <time.h>
#include "base/thread.h"

using namespace std;
using namespace base;

test_unidb::test_unidb()
	: db_(NULL)
	, t1(NULL)
	, t2(NULL)
{
}

test_unidb::~test_unidb()
{
	if(t1) {
		delete t1;
	}
	if(t2) {
		delete t2;
	}
}

void test_unidb::start_test()
{
	db_ = new naut::unidb("mysql");

	char buffer[2048];
	int record_count = 2;

	naut::unidb_param param;
	param.host = "192.168.137.35";
	param.port = 3306;
	param.user = "root";
	param.password = "000000";
	param.charset = "utf8";
	param.database_name = "testdb";
	param.create_database_if_not_exists = true;
	param.recreate_database_if_exists = false;

	naut::unidb_conn* conn = NULL;

	int ret = db_->open(param);
	if (ret != 0) {
		printf("open database error:%d\n", ret);
		goto end;
	}

	conn = new naut::unidb_conn(db_);
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
	    ) ENGINE=INNODB AUTO_INCREMENT=5525342 DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;");
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
		sprintf(buffer, "INSERT INTO `hqetf` (`avr_price`,`deal_amount`, `date`) VALUES (%f, %d, '%s');",
				i + 3900.0, i, "今天下雨了");
		conn->execute(buffer);
	}

	conn->begin_transaction();
	for (int i = 0; i < record_count; i++) {
		sprintf(buffer, "INSERT INTO `hqetf` (`avr_price`,`deal_amount`, `date`) VALUES (%f, %d, '%s');",
				i + 4900.0, i, "今天下雨了");
		conn->execute(buffer);
	}
	conn->rollback();

	for (int i = 0; i < record_count; i++) {
		sprintf(buffer, "INSERT INTO `hqetf` (`avr_price`,`deal_amount`, `date`) VALUES (%f, %d, '%s');",
				i + 5900.0, i, "今天下雨了");
		conn->execute(buffer);
	}

	conn->begin_transaction();
	for (int i = 0; i < record_count; i++) {
		sprintf(buffer, "INSERT INTO `hqetf` (`avr_price`,`deal_amount`, `date`) VALUES (%f, %d, '%s');",
				i + 6900.0, i, "今天下雨了");
		conn->execute(buffer);
	}
	conn->commit();


	for (int i = 0; i < record_count; i++) {
		sprintf(buffer, "INSERT INTO `hqetf` (`avr_price`,`deal_amount`, `date`) VALUES (%f, %d, '%s');",
				i + 7900.0, i, "今天下雨了");
		conn->execute(buffer);
	}


	/* query records */
	if (conn->query("select * from `hqetf`")) {
		int record_id = 0;
		while (conn->fetch_row()) {
			printf("id:%d avr_price:%lf, deal_amount:%ld, date:%s\n",
					record_id, conn->get_double("avr_price"),
					conn->get_long("deal_amount"),
					conn->get_string("date"));
			print_hex(conn->get_string("date"));
		}
	}

	/* check charset */
	check_charset();

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

void test_unidb::check_charset()
{
	printf("start check charset\n");

	iconv_t ih = iconv_open("utf-8", "gbk");

	db_ = new naut::unidb("mysql");

	naut::unidb_param param;
	param.host = "192.168.137.35";
	param.port = 3306;
	param.user = "root";
	param.password = "000000";
	param.charset = "gbk";
	param.database_name = "testdb";
	param.create_database_if_not_exists = true;
	param.recreate_database_if_exists = false;

	naut::unidb_conn* conn = NULL;

	LABEL_SCOPE_START;

	int ret = db_->open(param);
	if (ret != 0) {
		printf("open database error:%d\n", ret);
		goto end;
	}

	conn = new naut::unidb_conn(db_);
	if (!conn->init_conn()) {
		printf("initialize unidb conn failed:%d\n", conn->get_errno());
		goto end;
	}

	char* in_buffer = new char[205];
	char* out_buffer = new char[2050];
	char* out_buffer2 = out_buffer;
	/* query records */
	if (conn->query("select * from `hqetf`")) {
		int record_id = 0;
		while (conn->fetch_row()) {
			printf("id:%d avr_price:%f, deal_amount:%ld, date:%s\n",
					record_id, conn->get_double("avr_price"),
					conn->get_long("deal_amount"),
					conn->get_string("date"));
			print_hex(conn->get_string("date"));
			std::string date = conn->get_string("date");
			base::size_t out_length = 2048;
			base::size_t in_length = date.length();
			memcpy(in_buffer, date.c_str(), in_length);
			iconv(ih, &in_buffer, &in_length, &out_buffer, &out_length);
			printf("date:%s, %ld\n", out_buffer2, strlen(out_buffer2));
		}
	}

	printf("end of check charset\n");
	getchar();

	LABEL_SCOPE_END;

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

void test_unidb::print_hex(const char* text)
{
	while (*text != 0x00) {
		printf("%02X", *text);
		text++;
	}
	printf("\n");
}

void test_unidb::test_query(void* args)
{
	int count = *((int*)args);
	naut::unidb* db_ = new naut::unidb("mysql");

	char buffer[2048];
	int record_count = 2;

	naut::unidb_param param;
	param.host = "192.168.56.133";
	param.port = 3306;
	param.user = "test";
	param.password = "123456";
	param.charset = "utf8";
	param.database_name = "SGXQ";
	param.create_database_if_not_exists = true;
	param.recreate_database_if_exists = false;

	naut::unidb_conn* conn = NULL;

	int ret = db_->open(param);
	if (ret != 0) {
		printf("open database error:%d\n", ret);
		goto end;
	}

	conn = new naut::unidb_conn(db_);
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
	    ) ENGINE=INNODB AUTO_INCREMENT=5525342 DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;");
	if (!conn->execute(buffer)) {
		printf("create table failed\n");
	}

	/* delete all records */
//	if (!conn->execute("delete from `hqetf`")) {
//		printf("delete from hqetf failed\n");
//	}


	for (int i = count ; i < count + 10000; i++) {
		int ret1 = 0;
				ret1 =	conn->query("select * from `hqetf`");


		sprintf(buffer, "INSERT INTO `hqetf` (`avr_price`,`deal_amount`, `date`) VALUES (%f, %d, '%s');",
				i + 7900.0, i, "今天下雨了");
	 int ret2 =	conn->execute(buffer);

	 printf("i: %d, ret1: %d, ret2:%d \n", i, ret1, ret2);
	}
	/* check charset */
	// check_charset();

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

void test_unidb::start_multi_thread()
{
	int a1= 100000;
	int a2= 500000;
	t1 = new thread();
	t2 = new thread();
	t1->create(&test_unidb::test_query, (void*)&a1);
	t2->create(&test_unidb::test_query, (void*)&a2);
	t1->join();
	t2->join();
}

void test_unidb::insert_db_test()
{
	db_ = new naut::unidb("mysql");

	char buffer[2048];
	int record_count = 2;

	naut::unidb_param param;
	param.host = "192.168.4.65";
	param.port = 3306;
	param.user = "wcp";
	param.password = "wcp@2FRsenHkDsjfu7ASB3";
	param.charset = "utf8";
	param.database_name = "odfuture";
	param.create_database_if_not_exists = true;
	param.recreate_database_if_exists = false;

	naut::unidb_conn* conn = NULL;

	int ret = db_->open(param);
	if (ret != 0) {
		printf("open database error:%d\n", ret);
		goto end333;
	}

	conn = new naut::unidb_conn(db_);
	if (!conn->init_conn()) {
		printf("initialize unidb conn failed:%d\n", conn->get_errno());
		goto end333;
	}
	//a8t151204:642_2483,	a8t151204	光明联盟	0	2015-12-04	2015-12-04	00014990	1	CN1512	CN1512	1	1	10300	11640.5	1 1449211306	2015-12-04 14:41:46	2	1	-12.5	-0.001214	9524.5	10287.5	1449211368	2015-12-04 14:42:48	0	1	50	240	0	2	1449211302
	while(true)
	{
		string table_name = "test_insert";
		string user_name = "a8t151204:642_2483";
		string group_name = "a8t151204";
		string nick_name = "yushen";
		double initfund = 10.0;
		string start_date = "2015-12-04";
		string end_date = "2015-12-04";
		string account = "00001496";
		long must_deal = 0;
		string stock_code = "SGXQ";
		string stock_name = "CN1512";
	    long direction = 1;
	    long entrust_type = 1;

		time_t tt = time(NULL);
		tm* t = localtime(&tt);
		char buf[200];
		memset(buf, 0, 200);
		char sql[4096];
		memset(sql, 0 , 4096);
		sprintf(buf, "%d-%02d-%02d %02d:%02d:%02d",
					t->tm_year +1900,
					t->tm_mon +1,
					t->tm_mday,
					t->tm_hour,
					t->tm_min,
					t->tm_sec);
		sprintf(sql,"insert into %s(user_name,group_name,nickname,init_fund,start_date,"
			"end_date,account,must_deal,stock_code,stock_name,direction,entrust_type,"
			"start_price,start_order_price,stock_amount,start_time,start_time_t,status,ordertype,"
			"profit,profit_rate,clear_order_price,clear_price,cancel_order,"
			"multipler,zs_rate,zy_rate,now_sell,send_status,start_order_time) values('%s','%s','%s',%lf,'%s','%s','%s',%ld,"
			"'%s','%s',%ld,%ld,%lf,%lf,%d,%ld,'%s',%d,%ld,%lf,%lf,%lf,%lf,%ld,%d,%lf,%lf,%d,%d,%ld)",
			table_name.c_str(),
			user_name.c_str(),
			group_name.c_str(),
			nick_name.c_str(),
			initfund,
			start_date.c_str(),
			end_date.c_str(),
			account.c_str(),
			must_deal,
			stock_code.c_str(),
			//dict["stock_name"].string_value().c_str(),
			stock_name.c_str(),
			direction,
			entrust_type,
			(double)0.0,
				(double)0.0,
			(int)1,(long)tt,buf,0,(long)1,(double)0.0,double(0.0),(double)0,(double)0,(long)0,1,
			(double)0.0,(double)0.0,0,0,(long)tt);


//          (int)1,(long)now,time_to_string(now, stime, sizeof(stime)),0,dict["order_type"].integer_value(),(double)0.0,double(0.0),(double)0,(double)0,(long)0,1,
//          dict["zs_rate"].double_value(),dict["zy_rate"].double_value(),0,0,dict["order_time"].integer_value());



			if(!conn->execute(sql))
			{
				printf("add_stocker %s failed\n",sql);
			}
			base::thread::sleep(1);
	}
	printf("end of dbtest\n");

end333:
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

void test_unidb::start_redis_test()
{
    db_ = new naut::unidb("redis");

    char buffer[2048];
    int record_count = 0;
    std::string sql = "";

    naut::unidb_param param;
    param.host = "192.168.4.65";
    param.port = 6379;
    param.user = "";
    param.password = "888888";
    param.charset = "utf8";
    param.database_name = "0";
    param.create_database_if_not_exists = true;
    param.recreate_database_if_exists = false;

    naut::unidb_conn* conn = NULL;
    base::dictionary dict;

    int ret = db_->open(param);
    if (ret != 0) {
        printf("open database error:%d\n", ret);
        goto end;
    }

    conn = new naut::unidb_conn(db_);
    if (!conn->init_conn()) {
        printf("initialize unidb conn failed:%d\n", conn->get_errno());
        goto end;
    }

    memset(buffer, 0 , 2048);

    //对String操作的命令 set key value
    sql = "set str_test_key1 aaaa";
    if (!conn->execute(sql.c_str())) {
        printf("create table failed\n");
        return;
    } else {
        printf("sql:[%s] success~~~\n", sql.c_str());
    }

    // 对String操作的命令 get key
    dict.clear();
    dict["cmd"] = "get";
    dict["key"] = "str_test_key1";
//    dict["field"] = "name 1111";

    if (conn->query(base::djson::dict2str(dict).c_str())) {
        if (conn->fetch_row()) {
            printf("get str_test_key1:%s\n",
                    conn->get_string("str_test_key1"));
        }
    }

    //对List操作的命令 rpush key value
    sql = "rpush list_test_key1 lsitaaaa1";
    if (!conn->execute(sql.c_str())) {
        printf("create table failed\n");
        return;
    } else {
        printf("sql:[%s] success~~~\n", sql.c_str());
    }

    sql = "rpush list_test_key1 lsitaaaa2";
    if (!conn->execute(sql.c_str())) {
        printf("create table failed\n");
        return;
    } else {
        printf("sql:[%s] success~~~\n", sql.c_str());
    }

    // 对List操作的命令 lrange(key, start, end)：返回名称为key的list中start至end之间的元素
    dict.clear();
    dict["cmd"] = "lrange";
    dict["key"] = "list_test_key1";
    dict["param"] = "0 -1";
    if (conn->query(base::djson::dict2str(dict).c_str())) {
        if (conn->fetch_row()) {
            printf("lrange list_test_key1:%s\n",
                    conn->get_string("list_test_key1"));
        }
    }

    // 对List操作的命令 llen key , lpop key
    dict.clear();
    dict["cmd"] = "llen";
    dict["key"] = "list_test_key1";
    if (conn->query(base::djson::dict2str(dict).c_str())) {
        if (conn->fetch_row()) {
            record_count = conn->get_long("list_test_key1");
            printf("llen list_test_key1:%s, record_count:%d\n",
                    conn->get_string("list_test_key1"), record_count);
        }
    }
    for (int i = 0; i < record_count; i++) {
        dict.clear();
        dict["cmd"] = "lpop";
        dict["key"] = "list_test_key1";
        if (conn->query(base::djson::dict2str(dict).c_str())) {
            if (conn->fetch_row()) {
                printf("lpop list_test_key1: i:[%d], value:[%s]\n", i,
                        conn->get_string("list_test_key1"));
            }
        }
    }

    //对Set操作的命令 sadd key member
    sql = "sadd set_test_key1 setaaaa1";
    if (!conn->execute(sql.c_str())) {
        printf("create table failed\n");
        return;
    } else {
        printf("sql:[%s] success~~~\n", sql.c_str());
    }

    sql = "sadd set_test_key1 setaaaa2";
    if (!conn->execute(sql.c_str())) {
        printf("create table failed\n");
        return;
    } else {
        printf("sql:[%s] success~~~\n", sql.c_str());
    }

    // 对Set操作的命令 scard key , spop key
    dict.clear();
    dict["cmd"] = "scard";
    dict["key"] = "set_test_key1";
    if (conn->query(base::djson::dict2str(dict).c_str())) {
        if (conn->fetch_row()) {
            record_count = conn->get_long("set_test_key1");
            printf("llen set_test_key1:%s, record_count:%d\n",
                    conn->get_string("set_test_key1"), record_count);
        }
    }
    for (int i = 0; i < record_count; i++) {
        dict.clear();
        dict["cmd"] = "spop";
        dict["key"] = "set_test_key1";
        if (conn->query(base::djson::dict2str(dict).c_str())) {
            if (conn->fetch_row()) {
                printf("spop set_test_key1: i:[%d], value:[%s]\n", i,
                        conn->get_string("set_test_key1"));
            }
        }
    }


    //对Hash操作的命令 hset(key, field, value)：向名称为key的hash中添加元素field
    sql = "hset hash_test_key1 name david";
    if (!conn->execute(sql.c_str())) {
        printf("create table failed\n");
        return;
    } else {
        printf("sql:[%s] success~~~\n", sql.c_str());
    }

    sql = "hmset hash_test_key1 age 27 com wcp";
    if (!conn->execute(sql.c_str())) {
        printf("create table failed\n");
        return;
    } else {
        printf("sql:[%s] success~~~\n", sql.c_str());
    }

    // 对Hash操作的命令 hmget(key, (fields))：返回名称为key的hash中field i对应的value
    dict.clear();
    dict["cmd"] = "hmget";
    dict["key"] = "hash_test_key1";
    dict["field"] = "name age com";
    if (conn->query(base::djson::dict2str(dict).c_str())) {
        if (conn->fetch_row()) {
            printf("hmget hash_test_key1: name:%s age:%d com:%s\n",
                    conn->get_string("name"),
                    conn->get_long("age"),
                    conn->get_string("com"));
        }
    }

    // 对Hash操作的命令 hgetall(key)：返回名称为key的hash中所有的键（field）及其对应的value
    dict.clear();
    dict["cmd"] = "hgetall";
    dict["key"] = "hash_test_key1";
    if (conn->query(base::djson::dict2str(dict).c_str())) {
        if (conn->fetch_row()) {
            printf("hgetall hash_test_key1: %s\n",
                    conn->get_string("hash_test_key1"));
        }
    }

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
