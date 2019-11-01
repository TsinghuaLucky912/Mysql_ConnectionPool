#pragma once
/*
2019年9月15日16:37:31
主要是：

与MySQL数据库增删改查相关的  头文件只定义，源文件只实现
*/
#ifdef _MSC_VER
#ifdef _WIN64
#include <WinSock2.h>
#elif _WIN32
#include <winsock.h>
#endif
#endif


#include <string>
#include <mysql.h>
#include <ctime>
using namespace std;
// 数据库操作类
class Connection
{
public:
	// 初始化数据库连接
	Connection();

	// 释放数据库连接资源
	~Connection();

	// 连接数据库
	bool connect(string ip, //IP地址
		unsigned short port, //端口号
		string username,//用户名
		string password,//密码
		string dbname);//数据库名

	// 更新操作 insert、delete、update
	bool update(string sql);

	// 查询操作 select
	MYSQL_RES* query(string sql);

	//刷新一下 连接的起始的空闲时间点
	void refreshAliveTime()
	{
		_alivetime = clock();//开始记录
	}
	//返回存活的总时间
	clock_t getAliveTime()const
	{
		return (clock() - _alivetime);
	}
private:
	//oracle给我们封装好的
	MYSQL* _conn; // 表示和MySQL Server的一条连接

	clock_t _alivetime;//记录进入空闲状态后的起始存活时间，进入队列了
	//上面这个是 需要在进入队列的地方都记录一下的 进入队列就可以空闲
};