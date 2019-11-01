#pragma once
/*
2019��9��15��16:38:44
ʵ�ֵ��� ���ӳع���ģ��

����ģʽ������ϵͳ�����ӳض������һ�����ɡ������ݿ��������Ļ������Ǵ�ͬһ�����ӳ�����
��ȡ���Ӿ����ˡ����������Ƕ��߳�ʵ�ֵģ������ж���߳�ȥ��ȡ���ӳصĵ��������Ե�д���̰߳�ȫ�ĵ���


*/
#include <string>
#include <queue>
#include "Connection.h"
#include <mutex>
#include <atomic>//ԭ����
#include <thread>
#include <memory>//ʹ������ָ�� �Զ���ɾ��������Connection��������
#include <functional>//�������Ǻ�������

#include <condition_variable>//��Ҫ�̼߳�ͨ�Ų���
using namespace std;
class ConnectionPool
{
public:
	//���ṩһ���ӿڣ���̬���� �������ڶ���ĵ���
	//��ȡ���ӳض���ʵ��
	static ConnectionPool* getConnectionPool();

	//���ⲿ�ṩ�ӿڣ������ӳ������ȡһ�����õĿ�������
	//������Կ���ֱ�ӷ���һ������ָ�룬ʹ��������ָ��  ����ָ����������Զ�������
	//����ȥ�ͷ�Connection  ���������ָ���ض���һ��ɾ������������ʱ�� ���ǰ������ͷŵ�
	//���ǹ黹�����ӳء������Ͳ���ȥ���� closeConnection֮��ķ���
	//�����ʵ�� ��Ҫ����������û��Ĳ������û���ɵ
	//����ʹ������ָ�� �û����õ�ʱ������ָ���Զ���ɾ�������Connection�������� ����delete
	shared_ptr<Connection>getConnection();


private:
	ConnectionPool();//��һ�� ���캯��˽�л�

	//һ����˵ �����ļ������ƶ���Ĭ�ϵģ�����ֻҪ�������ļ���������
	//����Ĵ���ͻ��Զ�����ӽ���

	//�������ļ��м���������
	bool loadConfigFile();

	//�����ڶ������߳��У�ר�Ÿ�������������
	/*
	����һ����ͨ�ĳ�Ա���������������ڶ���
	Ϊ�˰�produceConnectionTask��Ƴ��̺߳��������thisָ����Ҫ�Ķ��� ����
	������Ҫ�������������Ǻ�������
	����д����ĳ�Ա���������Ժܷ���ķ��ʵ�ǰ����ĳ�Ա����
	*/
	void produceConnectionTask();

	//ɨ�賬��maxIdleTimeʱ��Ŀ������ӣ����ж�������ӻ���
	void scannerConnectionTask();//�̺߳���  ��ʱɨ���߳�

	string _ip;//���ݿ��IP��ַ
	unsigned short _port;//���ݿ�Ķ˿ں� 3306
	string _username;//MySQL��¼�û���
	string _password;//MySQL��¼����
	string _dbname;//���ӵ����ݿ�����
	//���ӳص���Ϣ
	int _initSize;//��ʼ������
	int _maxSize;//���������
	int _maxIdleTime;//������ʱ��
	int _connectionTimeout;//��ȡ���ӵĳ�ʱʱ��


	//���ӳ� ������Connection��  ��ָ�������
	//����������еĲ�������һ���̰߳�ȫ������
	queue<Connection*>_connectionQue;

	mutex _queueMutex;//ά�����Ӷ����̰߳�ȫ�Ļ�����

	atomic_int _connectionNum;//��¼��������connection���ӵ�������
	//���� ����ԭ�Ӳ�����++ --�����̰߳�ȫ��

	condition_variable cv;//�����������������������������̣߳��������߳�ͨ��
};

















