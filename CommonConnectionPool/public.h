#pragma once
/*
2019��9��15��16:35:27

public.hͷ�ļ�
��ţ��������Ŀ�ĸ����ļ������ �� ���ͣ�������Դ�ļ�ֱ��ʹ��

*/
#include <iostream>

//������һ�� ��־��ӡ
#define LOG(str) \
	std::cout<<__FILE__<<":"<<__LINE__<<" "<<__TIMESTAMP__<<" : "<<str<<std::endl;
//���� �� �ĸ��ļ�       ��һ��         ʲôʱ���������־