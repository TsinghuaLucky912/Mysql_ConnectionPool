#pragma once
/*
2019年9月15日16:38:44
实现的是 连接池功能模块

单例模式：整个系统的连接池对象出现一个即可。有数据库操作需求的话，都是从同一个连接池里面
获取连接就行了。但服务器是多线程实现的，定会有多个线程去获取连接池的单例，所以得写成线程安全的单例


*/
#include <string>
#include <queue>
#include "Connection.h"
#include <mutex>
#include <atomic>//原子类
#include <thread>
#include <memory>//使用智能指针 自定义删除器，将Connection返还池中
#include <functional>//绑定器都是函数对象

#include <condition_variable>//需要线程间通信操作
using namespace std;
class ConnectionPool
{
public:
	//得提供一个接口，静态方法 不依赖于对象的调用
	//获取连接池对象实例
	static ConnectionPool* getConnectionPool();

	//给外部提供接口，从连接池里面获取一个可用的空闲连接
	//这里可以考虑直接返回一个智能指针，使用完智能指针  智能指针出作用域自动析构了
	//进而去释放Connection  给这个智能指针重定义一下删除器，析构的时候 不是把连接释放掉
	//而是归还给连接池。这样就不用去定义 closeConnection之类的方法
	//软件的实现 不要过多的依赖用户的操作，用户最傻
	//这里使用智能指针 用户不用的时候，智能指针自定义删除器会把Connection返还池中 不是delete
	shared_ptr<Connection>getConnection();


private:
	ConnectionPool();//第一步 构造函数私有化

	//一般来说 配置文件的名称都是默认的，这里只要把配置文件给到这里
	//程序的代码就会自动地添加进来

	//从配置文件中加载配置项
	bool loadConfigFile();

	//运行在独立的线程中，专门负责生产新连接
	/*
	这是一个普通的成员方法，调用依赖于对象
	为了把produceConnectionTask设计成线程函数，则把this指针需要的对象 绑定上
	所以需要绑定器，绑定器都是函数对象
	把它写成类的成员函数：可以很方便的访问当前对象的成员变量
	*/
	void produceConnectionTask();

	//扫描超过maxIdleTime时间的空闲连接，进行多余的连接回收
	void scannerConnectionTask();//线程函数  定时扫描线程

	string _ip;//数据库的IP地址
	unsigned short _port;//数据库的端口号 3306
	string _username;//MySQL登录用户名
	string _password;//MySQL登录密码
	string _dbname;//连接的数据库名字
	//连接池的信息
	int _initSize;//初始连接量
	int _maxSize;//最大连接量
	int _maxIdleTime;//最大空闲时间
	int _connectionTimeout;//获取连接的超时时间


	//连接池 放连接Connection的  存指针就行了
	//但是这个队列的操作得是一个线程安全的容器
	queue<Connection*>_connectionQue;

	mutex _queueMutex;//维持连接队列线程安全的互斥锁

	atomic_int _connectionNum;//记录所创建的connection连接的总数量
	//上面 基于原子操作，++ --都是线程安全的

	condition_variable cv;//设置条件变量，用于连接生产者线程，消费者线程通信
};

















