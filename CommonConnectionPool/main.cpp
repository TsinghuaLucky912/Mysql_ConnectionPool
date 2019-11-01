
/**══════════════════════════════════╗
*作    者：songjinzhou                                                 ║
*CSND地址：https://blog.csdn.net/weixin_43949535                       ║
***GitHub：https://github.com/TsinghuaLucky912/My_own_C-_study_and_blog║
*═══════════════════════════════════╣
*创建时间：2019年9月15日16:30:42                                                            
*功能描述：main.cpp是本项目的所有测试代码的地方                                                            
*                                                                      
*                                                                      
*═══════════════════════════════════╣
*结束时间: 2019年9月16日18:04:53                                                            
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
#include "Connection.h"
#include "CommonConnectionPool.h"
#include <string>
using namespace std;

int main()
{
	//这里需要事先连接一下 数据库
	Connection conn;//定义一个Connection对象
	conn.connect("127.0.0.1", 3306, "root", "", "chat");
	/*
	Connection conn;//定义一个Connection对象
	char sql[1024] = { 0 };//下面是自己写的一条SQL语句
	sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')", "zhangsan", 20, "male");
	
	conn.connect("127.0.0.1", 3306, "root", "", "chat");
	conn.update(sql);
	
	注：conn是一个栈上的对象，出了作用域 自动析构。在析构里面就可以完成
	与服务器连接断开 回收conn对象的成员变量指向的资源 
	发起与MySQL server的四次挥手
	*/
	clock_t begin = clock();
	thread t1([]() {
	
		for (int i = 0; i < 250; ++i)//1000个人做业务
		{
			Connection conn;//定义一个Connection局部对象  出作用域的  这个得放在里面
			char sql[512] = { 0 };//下面是自己写的一条SQL语句
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')", "zhangsan", 20, "male");

			conn.connect("127.0.0.1", 3306, "root", "", "chat");
			conn.update(sql);
		}
		});
	thread t2([]() {
		
		for (int i = 0; i < 250; ++i)//1000个人做业务
		{
			Connection conn;//定义一个Connection局部对象  出作用域的  这个得放在里面
			char sql[512] = { 0 };//下面是自己写的一条SQL语句
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')", "zhangsan", 20, "male");

			conn.connect("127.0.0.1", 3306, "root", "", "chat");
			conn.update(sql);
		}
		});
	thread t3([]() {
		
		for (int i = 0; i < 250; ++i)//1000个人做业务
		{
			Connection conn;//定义一个Connection局部对象  出作用域的  这个得放在里面
			char sql[512] = { 0 };//下面是自己写的一条SQL语句
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')", "zhangsan", 20, "male");

			conn.connect("127.0.0.1", 3306, "root", "", "chat");
			conn.update(sql);
		}
		});
	thread t4([]() {
		
		for (int i = 0; i < 250; ++i)//1000个人做业务
		{
			Connection conn;//定义一个Connection局部对象  出作用域的  这个得放在里面
			char sql[512] = { 0 };//下面是自己写的一条SQL语句
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')", "zhangsan", 20, "male");

			conn.connect("127.0.0.1", 3306, "root", "", "chat");
			conn.update(sql);
		}
		});
	t1.join();
	t2.join();
	t3.join();
	t4.join();
	clock_t end = clock();
	cout << "运行时间：" << (end - begin) << "ms" << endl;


	return 0;
}
/**
*备用注释：
*
*
*
*/
