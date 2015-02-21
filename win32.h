#pragma once
#include "common.h"

#include <Windows.h>
#include <Psapi.h>
#include <TlHelp32.h>

//#include "pcm/cpucounters.h"
//#include "pcm/utils.h"


class Win32Info : public IInfo{
public:
	void print()  {};
};

class Win32Report : public IReport{
private:
	//vector<Token*> t;
	void update();
public:

	double processCpuUsage;
	double systemCpuUsage;

	size_t workingSetSize;
	size_t pagefileUsage;

	unsigned __int64 readOperationCountDiff;
	unsigned __int64 writeOperationCountDiff;
	unsigned __int64 otherOperationCountDiff;
	unsigned __int64 readTransferCountDiff;
	unsigned __int64 writeTransferCountDiff;
	unsigned __int64 otherTransferCountDiff;

	Win32Report() {};
	~Win32Report();
	void reset() {};
	string header();
	string csv();
	friend ostream& operator<<(ostream& out, Win32Report& p);
	friend ostream& operator<<(ostream& out, Win32Report* p);
};

ostream& operator<<(ostream& out, Win32Report& p){
	out << p.csv();
	return out;
}

ostream& operator<<(ostream& out, Win32Report* p){
	out << p->csv();
	return out;
}

void Win32Report::update(){
	for (int i = 0; i < t.size(); i++){
		if (t[i] != NULL)
			delete t[i];
	}
	t.clear();

	t.push_back(new Token("CPU_Usage_Of_Process", "% 20.2f", 21));
	t.push_back(new Token("CPU_Usage_Of_System", "% 19.2f", 20));
	
	t.push_back(new Token("Workingset_Of_Process", "% 21lu", 22));
	t.push_back(new Token("Pagefile_Usage_Of_Process", "% 25lu", 26));
	
	t.push_back(new Token("IO_Read_Operation_Count", "% 23I64u", 24));
	t.push_back(new Token("IO_Write_Operation_Count", "% 24I64u", 25));
	t.push_back(new Token("IO_Other_Operation_Count", "% 24I64u", 25));
	t.push_back(new Token("IO_Read_Operation_Bytes", "% 23I64u", 24));
	t.push_back(new Token("IO_Write_Operation_Bytes", "% 24I64u", 25));
	t.push_back(new Token("IO_Other_Operation_Bytes", "% 24I64u", 25));

	t[0]->set(processCpuUsage);
	t[1]->set(systemCpuUsage);
	
	t[2]->set(workingSetSize);
	t[3]->set(pagefileUsage);
	
	t[4]->set(readOperationCountDiff);
	t[5]->set(writeOperationCountDiff);
	t[6]->set(otherOperationCountDiff);
	
	t[7]->set(readTransferCountDiff);
	t[8]->set(writeTransferCountDiff);
	t[9]->set(otherTransferCountDiff);
}

Win32Report::~Win32Report(){
	for (int i = 0; i < t.size(); i++){
		if (t[i] != NULL)
			delete t[i];
	}
}

string Win32Report::header(){
	stringstream ss;
	this->update();

	for (int i = 0; i < t.size(); i++){
		ss << t[i]->h();
		if ((i + 1) != t.size())
			ss << ",";
	}
	return ss.str();
}

string Win32Report::csv(){
	stringstream ss;
	this->update();

	for (int i = 0; i < t.size(); i++){
		ss << t[i]->e();
		if ((i + 1) != t.size())
			ss << ",";
	}
	return ss.str();
}

class Win32 : public ISensor{
private:
	OSVERSIONINFO OsVersionInfo;
	HANDLE hProcess;
	string ProcessImageName;
	
	//system total times
	FILETIME prevSysIdle;
	FILETIME prevSysKernel;
	FILETIME prevSysUser;
	//process times
	FILETIME prevProcKernel;
	FILETIME prevProcUser;

	//process io counter
	ULONGLONG prevReadOperationCount;
	ULONGLONG prevWriteOperationCount;
	ULONGLONG prevOtherOperationCount;
	ULONGLONG prevReadTransferCount;
	ULONGLONG prevWriteTransferCount;
	ULONGLONG prevOtherTransferCount;

	ULONGLONG lastRun;

	PROCESS_MEMORY_COUNTERS pmc;
	IO_COUNTERS ic;

	ULONGLONG SubtractTimes(const FILETIME& ftA, const FILETIME& ftB);
public:
	Win32(HANDLE hProcess);
	//Win32(HANDLE )
	~Win32();
	int initialize();
	void getInfo(IInfo *p);
	void printInfo();
	void onUpdate(IReport *p);
	string getHeaderString();
	string getUpdateString();
};

ULONGLONG Win32::SubtractTimes(const FILETIME& ftA, const FILETIME& ftB){
	LARGE_INTEGER a, b;
	a.LowPart = ftA.dwLowDateTime;
	a.HighPart = ftA.dwHighDateTime;

	b.LowPart = ftB.dwLowDateTime;
	b.HighPart = ftB.dwHighDateTime;

	return a.QuadPart - b.QuadPart;
}

Win32::Win32(HANDLE hProcess){
	this->hProcess = hProcess;

	//retreive process image name
	LPWSTR wtemp = new WCHAR[512];
	DWORD charsCarried;
	QueryFullProcessImageName(this->hProcess, PROCESS_NAME_NATIVE, wtemp, &charsCarried);
	char temp[512];
	char defChar = ' ';
	WideCharToMultiByte(CP_ACP, 0, wtemp, -1, temp, 512, &defChar, NULL);
	this->ProcessImageName = temp;

	//get information fo os
	//GetVersionEx(&(this->OsVersionInfo));
	//do some stub

	//initialize delta values
	ZeroMemory(&prevSysKernel, sizeof(FILETIME));
	ZeroMemory(&prevSysUser, sizeof(FILETIME));
	ZeroMemory(&prevProcKernel, sizeof(FILETIME));
	ZeroMemory(&prevProcUser, sizeof(FILETIME));

	ZeroMemory(&prevReadOperationCount, sizeof(ULONGLONG));
	ZeroMemory(&prevWriteOperationCount, sizeof(ULONGLONG));
	ZeroMemory(&prevOtherOperationCount, sizeof(ULONGLONG));
	ZeroMemory(&prevReadTransferCount, sizeof(ULONGLONG));
	ZeroMemory(&prevWriteTransferCount, sizeof(ULONGLONG));
	ZeroMemory(&prevOtherTransferCount, sizeof(ULONGLONG));

//	this->workingSetSize = 0;
//	this->pagefileUsage = 0;
}


Win32::~Win32(){
	

}

int Win32::initialize(){
	ULONGLONG IOUsageBytes = 0;

	FILETIME ftSysIdle, ftSysKernel, ftSysUser;
	FILETIME ftProcCreation, ftProcExit, ftProcKernel, ftProcUser;

	if (!GetSystemTimes(&ftSysIdle, &ftSysKernel, &ftSysUser) ||
		!GetProcessTimes(hProcess, &ftProcCreation, &ftProcExit, &ftProcKernel, &ftProcUser)){
		return 0;
	}

	if (!GetProcessIoCounters(hProcess, &ic)){
		return 0;
	}

	this->prevSysIdle = ftSysIdle;
	this->prevSysKernel = ftSysKernel;
	this->prevSysUser = ftSysUser;
	//this->prevSysTotal = ftSysKernel + ftSysUser;
	this->prevProcKernel = ftProcKernel;
	this->prevProcUser = ftProcUser;

	this->prevReadOperationCount = ic.ReadOperationCount;
	this->prevWriteOperationCount = ic.WriteOperationCount;
	this->prevOtherOperationCount = ic.OtherOperationCount;
	this->prevReadTransferCount = ic.ReadTransferCount;
	this->prevWriteTransferCount = ic.WriteTransferCount;
	this->prevOtherTransferCount = ic.OtherTransferCount;

	lastRun = GetTickCount64();

	return 1;
}

void Win32::getInfo(IInfo *p){
	Win32Info *i = static_cast<Win32Info*>(p);
}

void Win32::printInfo(){
	Win32Info i;
	Win32::getInfo(&i);
}

void Win32::onUpdate(IReport *p){
	Win32Report* i = static_cast<Win32Report*>(p);
	i->reset();

	ULONGLONG IOUsageBytes = 0;

	FILETIME ftSysIdle, ftSysKernel, ftSysUser;
	FILETIME ftProcCreation, ftProcExit, ftProcKernel, ftProcUser;

	if (!GetSystemTimes(&ftSysIdle, &ftSysKernel, &ftSysUser) ||
		!GetProcessTimes(hProcess, &ftProcCreation, &ftProcExit, &ftProcKernel, &ftProcUser)) { return; }

	ULONGLONG ftSysKernelDiff = SubtractTimes(ftSysKernel, this->prevSysKernel);
	ULONGLONG ftSysUserDiff = SubtractTimes(ftSysUser, this->prevSysUser);
	ULONGLONG ftSysIdleDiff = SubtractTimes(ftSysIdle, this->prevSysIdle);
	long long kernelTotal = (long long)(ftSysKernelDiff - ftSysIdleDiff);

	ULONGLONG ftProcKernelDiff = SubtractTimes(ftProcKernel, this->prevProcKernel);
	ULONGLONG ftProcUserDiff = SubtractTimes(ftProcUser, this->prevProcUser);
	ULONGLONG nTotalSys = ftSysKernelDiff + ftSysUserDiff;
	ULONGLONG nTotalProc = ftProcKernelDiff + ftProcUserDiff;

	//이밑을 i 로 치환
	if (nTotalSys > 0){
		//this->cpuUsage = (float)((100.0 * nTotalProc) / nTotalSys);
		i->processCpuUsage = (double)((nTotalProc * 100.0) / nTotalSys);
		i->systemCpuUsage = (double)(((kernelTotal + ftSysUserDiff) * 100.) / nTotalSys);
	}
	else {
		//this->cpuUsage = 0.;
		i->processCpuUsage = 0.;
		i->systemCpuUsage = 0.;
	}

	this->prevSysIdle = ftSysIdle;
	this->prevSysKernel = ftSysKernel;
	this->prevSysUser = ftSysUser;
	//this->prevSysTotal = ftSysKernel + ftSysUser;


	this->prevProcKernel = ftProcKernel;
	this->prevProcUser = ftProcUser;

	//pi.CPULoad = this->cpuUsage;


	//***** ****** ***** ***** *****
	//IO Usage of process
	//***** ****** ***** ***** *****
	if (!GetProcessIoCounters(hProcess, &ic)) { return; }

	//이밑을 i 로 치환
	i->readOperationCountDiff = ic.ReadOperationCount - this->prevReadOperationCount;
	i->writeOperationCountDiff = ic.WriteOperationCount - this->prevWriteOperationCount;
	i->otherOperationCountDiff = ic.OtherOperationCount - this->prevOtherOperationCount;
	i->readTransferCountDiff = ic.ReadTransferCount - this->prevReadTransferCount;
	i->writeTransferCountDiff = ic.WriteTransferCount - this->prevWriteTransferCount;
	i->otherTransferCountDiff = ic.OtherTransferCount - this->prevOtherTransferCount;

	this->prevReadOperationCount = ic.ReadOperationCount;
	this->prevWriteOperationCount = ic.WriteOperationCount;
	this->prevOtherOperationCount = ic.OtherOperationCount;
	this->prevReadTransferCount = ic.ReadTransferCount;
	this->prevWriteTransferCount = ic.WriteTransferCount;
	this->prevOtherTransferCount = ic.OtherTransferCount;

	//***** ****** ***** ***** *****
	//Memory Usage of process
	//***** ****** ***** ***** *****
	if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))){
		i->workingSetSize = pmc.WorkingSetSize;
		i->pagefileUsage = pmc.PagefileUsage;
	}
	else {
		i->workingSetSize = -1;
		i->pagefileUsage = -1;
	}

	lastRun = GetTickCount64();
}

string Win32::getHeaderString(){ return string(); }
string Win32::getUpdateString(){ return string(); }