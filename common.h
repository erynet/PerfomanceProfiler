#pragma once
#include <iostream>
#include <iomanip>

#include <vector>
#include <map>
#include <string>
#include <sstream>


#include <math.h>
#include <iomanip>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cstring>
#include <sstream>

#include <cassert>

#include <Windows.h>

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

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
	//virtual friend ostream& operator<<(ostream& out, IReport& p) {};
	//virtual friend ostream& operator<<(ostream& out, IReport* p) {};
};

interface ISensor{
	virtual int initialize() = 0;
	virtual void getInfo(IInfo* p) = 0;
	virtual void printInfo() = 0;
	virtual void onUpdate(IReport* p) = 0;
	virtual string getHeaderString() = 0;
	virtual string getUpdateString() = 0;
	virtual ~ISensor() {};
};


//interface IToken{
//	virtual void set(int) = 0;
//	virtual void set(double) = 0;
//	virtual string h() = 0;
//	virtual string e() = 0;
//};


class Token{
private:
	string header;
	string format;
	unsigned int maxLength;

	int ivalue;
	unsigned int uivalue;
	double dvalue;
	string svalue;
	
	char buffer[128];
	unsigned int length;
	
public:
	int type;

	Token(string header, string format, unsigned int maxLength);
	void set(int value);
	void set(unsigned int value);
	void set(double value);
	void set(string value);
	string h();
	string e();
};

Token::Token(string header, string format, unsigned int maxLength){
	char temp[128];
	stringstream ss;
	ss << "% " << maxLength << "s";
	sprintf_s(temp, 128, ss.str().c_str(), header.c_str());
	this->header = header;

	this->format = format;
	this->maxLength = maxLength;
	this->length = header.length();
	//*(this->buffer) = "";

	this->type = 0;
}
void Token::set(int value){
	this->ivalue = value;
	this->type = 1;
}

void Token::set(unsigned int value){
	this->uivalue = value;
	this->type = 2;
}

void Token::set(double value){
	this->dvalue = value;
	this->type = 3;
}

void Token::set(string value){
	this->svalue = value;
	this->type = 4;
}

string Token::h(){
	return this->header;
}

string Token::e(){
	if (this->type == 1){
		sprintf_s(this->buffer, 128, this->format.c_str(), this->ivalue);
	}
	else if (this->type == 2){
		sprintf_s(this->buffer, 128, this->format.c_str(), this->uivalue);
	}
	else if (this->type == 3){
		sprintf_s(this->buffer, 128, this->format.c_str(), this->dvalue);
	}
	else if (this->type == 4){
		sprintf_s(this->buffer, 128, this->format.c_str(), this->svalue.c_str());
	}
	else {
		cout << "this->type : " << this->type << endl;
		assert(0);
	}
	return string(this->buffer);
}
