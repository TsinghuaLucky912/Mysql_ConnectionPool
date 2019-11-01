#pragma once
/*
2019��9��15��16:37:31
��Ҫ�ǣ�

��MySQL���ݿ���ɾ�Ĳ���ص�  ͷ�ļ�ֻ���壬Դ�ļ�ֻʵ��
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
// ���ݿ������
class Connection
{
public:
	// ��ʼ�����ݿ�����
	Connection();

	// �ͷ����ݿ�������Դ
	~Connection();

	// �������ݿ�
	bool connect(string ip, //IP��ַ
		unsigned short port, //�˿ں�
		string username,//�û���
		string password,//����
		string dbname);//���ݿ���

	// ���²��� insert��delete��update
	bool update(string sql);

	// ��ѯ���� select
	MYSQL_RES* query(string sql);

	//ˢ��һ�� ���ӵ���ʼ�Ŀ���ʱ���
	void refreshAliveTime()
	{
		_alivetime = clock();//��ʼ��¼
	}
	//���ش�����ʱ��
	clock_t getAliveTime()const
	{
		return (clock() - _alivetime);
	}
private:
	//oracle�����Ƿ�װ�õ�
	MYSQL* _conn; // ��ʾ��MySQL Server��һ������

	clock_t _alivetime;//��¼�������״̬�����ʼ���ʱ�䣬���������
	//��������� ��Ҫ�ڽ�����еĵط�����¼һ�µ� ������оͿ��Կ���
};