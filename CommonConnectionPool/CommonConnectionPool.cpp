//2019年9月15日18:42:49
#include "CommonConnectionPool.h"
#include "public.h"

//这就是一个 线程安全的懒汉单例函数接口
ConnectionPool* ConnectionPool::getConnectionPool()
{
	//对于静态局部对象而言 其初始化由编译器自动进行lock和unlock
	static ConnectionPool pool;

	//且静态局部对象 第一次运行到它的时候才去初始化，生成构造对象
	//所以没有获取这个对象实例时，getConnectionPool的唯一的实例是不会初始化产生的
	return &pool;
}

bool ConnectionPool::loadConfigFile()
{
	//从配置文件中加载配置项
	FILE* pf = fopen("mysql.ini", "r");
	if (pf == nullptr)
	{
		LOG("mysql.ini is not exist!");
		return false;
	}
	//如果文件存在	
	while (!feof(pf))//文件没有到末尾
	{
		char line[1024] = { 0 };//接收一行数据
		fgets(line, 1024, pf);
		string str = line;

		//配置项都是键值对key value
		int index = str.find('=', 0);//从0号位置开始 找到键值对的位置
		if (index == -1)//没找到  无效的配置项
		{
			continue;//读下一行
		}
		//username=root\n 后面还有这个回车
		int end_index = str.find('\n', index);//从index向后找回车

		string key = str.substr(0, index);//这是前面的键
		string value = str.substr(index + 1, end_index - 1 - index);//这是后面的值
		//下面就要将这些配置文件 写进连接池的成员变量里面
		if (key == "ip")
		{
			_ip = value;
		}
		else if (key == "port")
		{
			_port = atoi(value.c_str());
		}
		else if (key == "username")
		{
			_username = value;
		}
		else if (key == "password")
		{
			_password = value;
		}
		else if (key == "dbname")
		{
			_dbname = value;
		}
		else if (key == "initSize")
		{
			_initSize = atoi(value.c_str());
		}
		else if (key == "maxSize")
		{
			_maxSize = atoi(value.c_str());
		}
		else if (key == "maxIdleTime")
		{
			_maxIdleTime = atoi(value.c_str());
		}
		else if(key=="connectionTimeOut")
		{
			_connectionTimeout = atoi(value.c_str());
		}
	}
	return true;
}

/*
连接池的构造函数
它是在第一次单例对象在获取之后，调用这个构造函数
把初始的connection给建立起来了。
*/
ConnectionPool::ConnectionPool()
{
	//加载配置项
	if (loadConfigFile() == false)
	{
		return;//配置失败
	}
	//创建初始数量的连接    
	//这一块是整个系统 连接池启动时做的(没有线程可以去使用)，这块就不用考虑线程安全
	for (int i = 0; i < _initSize; ++i)
	{
		Connection* p = new Connection();
		//下面 创建新的连接 连接数据库
		p->connect(_ip, _port, _username, _password, _dbname);

		p->refreshAliveTime();//记录进入空闲的起始时间
		_connectionQue.push(p);//放入连接池
		_connectionNum++;
	}

	//下面就是 启动一个新的线程作为连接的生产者  生产者线程
	//在系统启动以后，连接池运行之后 对于队列的操作就得考虑线程安全了
	//C++上的thread就是在Linux上 底层调用的为pthread_create
	//不过这里需要传入一个线程函数，这个线程函数同pthread_create一样 都需要一个C接口
	//不能写成连接池里面的成员方法，但是写一个全局的接口 又无法访问ConnectionPool类的
	//成员变量，所以在ConnectionPool类里面添加produceConnectionTask
	//把当前对象 给绑定进去。不然成员方法produceConnectionTask是不能作为一个线程函数 
	thread produce(std::bind(&ConnectionPool::produceConnectionTask, this));
	produce.detach();//设置成守护线程

	//启动一个新的定时线程，扫描超过maxIdleTime时间的空闲连接，进行多余的连接回收
	thread scanner(std::bind(&ConnectionPool::scannerConnectionTask, this));
	scanner.detach();//主线程结束了，这些守护线程也就自动结束了
}

//扫描超过maxIdleTime时间的空闲连接，进行多余的连接回收
void ConnectionPool::scannerConnectionTask()
{
	while (1)//这是一个单独的线程   定时
	{
		//通过sleep模拟定时效果
		this_thread::sleep_for(chrono::seconds(_maxIdleTime));

		//扫描整个队列，释放多余的连接
		unique_lock<mutex>lock(_queueMutex);//操作队列 肯定要加锁
		while (_connectionNum > _initSize)
		{
			Connection* p = _connectionQue.front();
			if (p->getAliveTime() >= (_maxIdleTime * 1000))//从上一轮到这一轮 再没有被用到
			{
				//这个连接就要被释放
				_connectionQue.pop();
				_connectionNum--;
				delete p;//会调用Connection的析构函数  释放连接
			}
			else
			{
				break;//若是队头的 都没有超过最大空闲时间，后面的也不会
			}
		}
	}
}

//运行在独立的线程中，专门负责生产新连接
/*
	这是一个普通的成员方法，调用依赖于对象
	为了把produceConnectionTask设计成线程函数，则把this指针需要的对象 绑定上
	所以需要绑定器，绑定器都是函数对象
	把它写成类的成员函数：可以很方便的访问当前对象的成员变量
*/
void ConnectionPool::produceConnectionTask()
{
	//当发现队列是空的时候，就要去生产连接 前提是：没有超过_maxSize
	//这里面涉及到线程的通信操作
	//锁和条件变量 实现生产者消费者

	while (1)//一直在查看,循环  只要系统不关，连接池一直在使用 这个线程就一直在工作
	{
		//在unique_lock的构造函数里面，会调用锁_queueMutex的加锁
		unique_lock<mutex>lock(_queueMutex);//生产者加锁，那么消费者就拿不到这个锁

		while (!_connectionQue.empty())//队列不空，有连接
		{
			//生产线程进入等待状态，不占据CPU时间
			cv.wait(lock);//等待在这个条件变量上
			//进入等待状态之后，就会把锁进行释放
			//消费者线程可以拿到这把锁 去从队列里面取出连接
		}
		//若是队列为空，没有连接，那么消费者线程就会通知 notify生产者线程
		//生产者线程起来之后，发现线程池为空，则开始生产

		if (_connectionNum < _maxSize)//连接数量没有到达上限，则继续创建新的连接
		{
			// 若是 >=,则不创建
			Connection* p = new Connection();
			//下面 创建新的连接 连接数据库
			p->connect(_ip, _port, _username, _password, _dbname);

			p->refreshAliveTime();//记录进入空闲的起始时间
			_connectionQue.push(p);//放入连接池
			_connectionNum++;
		}

		//通知消费者线程，可以继续消费连接
		cv.notify_all();
		//消费者就从等待 调到阻塞状态
	}//出了这个括号，把_queueMutex锁 释放，消费者就可以拿到这把锁
}



/*
server app里面谁想用，就直接在那个线程里面调用一下这个连接池的getConnection
给外部提供接口，从连接池里面获取一个可用的空闲连接
这个没有产生单独线程，它是直接运行在服务器创建的线程里面
是给外部调用的  给消费者线程用的
*/
shared_ptr<Connection> ConnectionPool::getConnection()
{
	//涉及操作队列 需要先加锁 因为队列不是线程安全的
	unique_lock<mutex>lock(_queueMutex);
	while (_connectionQue.empty())
	{
		//等待这么长时间，有连接拿走；没有获取到，失败
		//用下面的  cv.wait_for(lock, chrono::milliseconds(_connectionTimeout));
		/*
		在这个时间之内，有生产者线程生产好的连接 就会直接通知它的
		或者是 其他的应用线程把连接使用完了，归还到队列里面，都会通知一下的
		*/
		if (cv_status::timeout == cv.wait_for(lock, chrono::milliseconds(_connectionTimeout)))
		{
			//但是从这个时间出来  这才是真正自己醒来的
			if (_connectionQue.empty())//依然是空的，自己醒来的 超时了
			{
				LOG("获取空闲连接超时...获取连接失败！");
				return nullptr;
			}
		}
		/*不是timeout,则说明是有人把它唤醒的 则再上去看看空不空
		若是还空，说明有一个可用 但是被人抢走了  那我再等

		若是no_timeout  这是自己醒的，那我失败
		*/
	}
	/*
	注：这里返还只能指针，但是用完之后 智能指针要析构
	智能指针 默认释放做法：把Connection资源给delete
	相当于调用Connection的析构函数，把Connection给关闭掉了
	这里需要自定义shared_ptr的释放资源的方式：把Connection归还到queue里面

	删除器是一个函数对象，也没必要去整个函数对象的类型出来
	直接使用lambda表达式，给shared_ptr第二个参数 传参
	*/
	//若是队列不为空，则创建一个智能指针
	shared_ptr<Connection>sp(_connectionQue.front(),
		[&](Connection*p) {//要把释放的连接指针 传进来(捕获外部的变量)，没有返回值 返回void
			
			//不过归还也得保证线程安全操作，这是在服务器应用线程做的
			unique_lock<mutex>lock(_queueMutex);//构造 加锁；出了作用域 析构 开锁

			p->refreshAliveTime();//记录进入空闲的起始时间
			_connectionQue.push(p);//在服务器应用线程里面用完了，也得把连接资源归还到这里
		});
	_connectionQue.pop();

	/*if (_connectionQue.empty())
	{*/
		cv.notify_all();//我消费了，我消费结束了，生产者可以生产了
		//谁消费了队列中的最后一个 连接，它就得负责通知 生产者可以生产了
		//这里不用加这个if判断也行，你把生产者唤醒了，人家发现队列不空，人家也不会生产
		//也不会搭理你  但是我觉得没必要老把人家唤醒
		//主要是 消费完连接以后，通知生产者线程去检测一下 若队列空了，赶紧生产
	//}	
	return sp;
}