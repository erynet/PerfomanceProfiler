#pragma once
#include <iostream>
#include <iomanip>

#include <vector>
#include <map>
#include <string>


#include <math.h>
#include <iomanip>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cstring>
#include <sstream>

#include <cassert>

#include <Windows.h>

using namespace std;

typedef int int32;
typedef long long int64;

typedef unsigned int uint32;
typedef unsigned long long uint64;



#define MAKE_SINGLETON( classname )			\
	private:								\
		classname() {}						\
		classname(const classname&);		\
		void operator=(const classname&);	\
	public:									\
		static classname& getInstance()		\
		{									\
			static classname instance;		\
			return &instance;				\
		}


//interface IConfig{
//	int colCount;
//	string header[32];
//	map < string, string > p;
//};

interface IInfo{
	virtual void print() = 0;
};


interface IReport{
	virtual string header() = 0;
	virtual string csv() = 0;
};

interface ISensor{
	//virtual void setConfig(void* p) = 0;
	virtual int initialize() = 0;
	virtual void getInfo(IInfo* p) = 0;
	virtual void printInfo() = 0;
	virtual void onUpdate(IReport* p) = 0;
	virtual string getHeaderString() = 0;
	virtual string getUpdateString() = 0;
	virtual ~ISensor() {};
};