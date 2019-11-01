
/**══════════════════════════════════╗
*作    者：songjinzhou                                                 ║
*CSND地址：https://blog.csdn.net/weixin_43949535                       ║
***GitHub：https://github.com/TsinghuaLucky912/My_own_C-_study_and_blog║
*═══════════════════════════════════╣
*创建时间：                                                            
*功能描述：这个主要是数据库操作的封装，把人家的API封装成我们的函数                                                           
*                                                                      
*                                                                      
*═══════════════════════════════════╣
*结束时间:                                                             
*═══════════════════════════════════╝
//                .-~~~~~~~~~-._       _.-~~~~~~~~~-.
//            __.'              ~.   .~              `.__
//          .'//              西南\./联大               \\`.
//        .'//                     |                     \\`.
//      .'// .-~"""""""~~~~-._     |     _,-~~~~"""""""~-. \\`.
//    .'//.-"                 `-.  |  .-'                 "-.\\`.
//  .'//______.============-..   \ | /   ..-============.______\\`.
//.'______________________________\|/______________________________`.
*/
#include <iostream>
#include "public.h"
#include "Connection.h"
using namespace std;


	
Connection::Connection()//构造函数
{
	_conn = mysql_init(nullptr);// 初始化数据库连接

	/*
	_conn 成员变量是一个指针，相当于给它指向这么一个结构体变量
	*/
}
// 释放数据库连接资源
Connection::~Connection()//析构函数
{
	if (_conn != nullptr)
		mysql_close(_conn);
}
	
bool Connection::connect(string ip, // 连接数据库
	unsigned short port, 
	string username, 
	string password,
	string dbname)
{
	//因为人家mysql_real_connect函数接收的都是 const char*指针，所以下面都得有c_str转换
	MYSQL* p = mysql_real_connect(_conn, ip.c_str(), username.c_str(),
		password.c_str(), dbname.c_str(), port, nullptr, 0);
	return p != nullptr;
}
// 更新操作 insert、delete、update这几个操作都调用update函数
bool Connection::update(string sql)
{
	if (mysql_query(_conn, sql.c_str()))
	{
		LOG("更新失败:" + sql);//更新失败 把SQL语句打印一下
		return false;
	}
	return true;
}

// 查询操作 select
MYSQL_RES* Connection::query(string sql)
{
	if (mysql_query(_conn, sql.c_str()))
	{
		LOG("查询失败:" + sql);
		return nullptr;
	}
	return mysql_use_result(_conn);
}

/**
*备用注释：
*
*
*
*/
