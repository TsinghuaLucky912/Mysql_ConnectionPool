#pragma once
/*
2019年9月15日16:35:27

public.h头文件
存放：把这个项目的各个文件共享的 宏 类型，给其他源文件直接使用

*/
#include <iostream>

//下面是一个 日志打印
#define LOG(str) \
	std::cout<<__FILE__<<":"<<__LINE__<<" "<<__TIMESTAMP__<<" : "<<str<<std::endl;
//这是 在 哪个文件       哪一行         什么时间输出的日志