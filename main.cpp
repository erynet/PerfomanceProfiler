#include "common.h"

#include "cpu.h"
#include "gpu.h"
#include "lpc.h"
#include "win32.h"
#include "d3d.h"

#include "console_util.h"

//#include "visitor.h"
using namespace std;

ConsoleUtil cu;
bool isFirstShow;
bool stopTheWorld;

string TimeStamp(){
	char c[128];
	SYSTEMTIME st;

	GetSystemTime(&st);
	sprintf_s(c, 128, "%4d/%02d/%02d %02d:%02d:%02d.%03d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

	return string(c);
}

BOOL CtrlHandler(DWORD fdwCtrlType){
	switch (fdwCtrlType){
	case CTRL_C_EVENT:
		Beep(750, 300);
		stopTheWorld = true;
		return true;
//	case CTRL_CLOSE_EVENT:
//		Beep(600, 200);
//		printf("Ctrl-Close event\n\n");
//		return true;
//
//	case CTRL_BREAK_EVENT:
//		Beep(900, 200);
//		printf("Ctrl-Break event\n\n");
//		return false;
//
//	case CTRL_LOGOFF_EVENT:
//		Beep(1000, 200);
//		printf("Ctrl-Logoff event\n\n");
//		return false;
//
//	case CTRL_SHUTDOWN_EVENT:
//		Beep(750, 500);
//		printf("Ctrl-Shutdown event\n\n");
//		return false;
	default:
		return false;
	}
}

void Show(CPUReport *rCPU, GPUReport *rGPU, Win32Report *rWin32){
	char temp[16];
	if (isFirstShow){
		isFirstShow = false;
		cu.Go(1, 0);
		cu.CC(1);
		cu << "Press Ctrl-C to stop";
		cu.CC(0);

		cu.Go(1, 2);
		cu << "CPU :      °C,        %(        %),        J";
		cu.Go(1, 3);
		cu << "GPU :      °C,        %,        %,        %,      Mhz,      Mhz,      Mhz";
	}
	cu.Go(1, 1);
	cu << TimeStamp();
	cu.CC(1);
	sprintf_s(temp, 16, "% 3d", rCPU->ThermalInfoOfSocket);
	cu.Go(8, 2);
	cu << temp;
	cu.CC(0);
	sprintf_s(temp, 16, "% 6.2lf", rWin32->processCpuUsage);
	cu.Go(17, 2);
	cu << temp;
	sprintf_s(temp, 16, "% 6.2lf", rWin32->systemCpuUsage);
	cu.Go(27, 2);
	cu << temp;
	sprintf_s(temp, 16, "% 6.2lf", rCPU->ConsumedJoules);
	cu.Go(38, 2);
	cu << temp;
	
	cu.CC(1);
	sprintf_s(temp, 16, "% 3d", rGPU->currentThermalGpu);
	cu.Go(8, 3);
	cu << temp;
	cu.CC(0);
	sprintf_s(temp, 16, "% 3d", rGPU->loadCore);
	cu.Go(20, 3);
	cu << temp;
	sprintf_s(temp, 16, "% 3d", rGPU->loadMemoryControler);
	cu.Go(30, 3);
	cu << temp;
	sprintf_s(temp, 16, "% 3d", rGPU->loadVideoEngine);
	cu.Go(40, 3);
	cu << temp;
	sprintf_s(temp, 16, "% 4d", rGPU->currentFreqCore / 1000);
	cu.Go(47, 3);
	cu << temp;
	sprintf_s(temp, 16, "% 4d", rGPU->currentFreqMemory / 1000);
	cu.Go(56, 3);
	cu << temp;
	sprintf_s(temp, 16, "% 4d", rGPU->currentFreqShader / 1000);
	cu.Go(66, 3);
	cu << temp;

	cu.Go(1, 4);
	cu << ".";
}

wchar_t* CharToWChar(const char* pstrSrc){
	size_t nLen = strlen(pstrSrc) + 1;
	wchar_t* pwstr = (LPWSTR)malloc(sizeof(wchar_t)* nLen);
	mbstowcs(pwstr, pstrSrc, nLen);
	return pwstr;
}

int AttachProcess(string Image, HANDLE& hProcess){
	HANDLE hProcessSnap;
	PROCESSENTRY32 pe32;
	int retryCount = 100;
	int commaCount = 0;
	bool isFirst = true;
	bool isSuccess = false;

	//take a snapshot of current process list
	while (!(retryCount == 0)){
		hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hProcessSnap == INVALID_HANDLE_VALUE)
			return 0;
		pe32.dwSize = sizeof(pe32);

		if (!Process32First(hProcessSnap, &pe32)){
			CloseHandle(hProcessSnap);
			return 0;
		}

		do{
			if (_wcsicmp(CharToWChar(Image.c_str()), pe32.szExeFile) == 0){
				hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, false, pe32.th32ProcessID);
				isSuccess = true;
				cu.Reset();
				break;
			}
		} while (Process32Next(hProcessSnap, &pe32));

		CloseHandle(hProcessSnap);

		if (isSuccess)
			break;

		//console io
		if (isFirst){
			isFirst = false;
			cu.Reset();
			cu.Go(2, 5);
			cu << " ";
			cu.Go(2, 2);
			cu << "Waiting for the process to running ";
		}
		else{
			if ((commaCount != 0) && ((commaCount % 5) == 0)){
				cu.GoR(-5, 0);
				cu << "     ";
				cu.GoR(-5, 0);
			}

			commaCount++;
			cu << ".";
		}
		retryCount--;
		Sleep(500);
	}
		
	if (hProcess == NULL){
		if (GetLastError() == 5){
			cu.Go(0, 0);
			cu << "Access is Denied";
		}
		return 0;
	}
	else
		return 1;
}

int main(int argc, char **argv){

	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	HANDLE hProcess = NULL;
	
	bool doLogging = false;
	stopTheWorld = false;
	ofstream log;

	stringstream ss;

	ios_base::sync_with_stdio(false);
	//redirect cerr, wcerr
	ofstream error("pf_info.txt");
	wofstream werror("pf_error.txt");
	streambuf *errbuf = std::cerr.rdbuf(error.rdbuf());
	wstreambuf *werrbuf = std::wcerr.rdbuf(werror.rdbuf());
	
	if (!SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE)){
		//do nothing ... 
	}

	cu.Reset();
	
	if (argc == 1){
		cout << "USAGE : pf.exe target.exe\n";
		cout << "        pf.exe target.exe output.csv";
		return 0;
	}
	else if (argc == 2){
		if (!AttachProcess(string(argv[1]), hProcess))
			return 0;
	}
	else if (argc >= 3){
		doLogging = true;
		log.open(argv[2]);
		clog.rdbuf(log.rdbuf());
		
		if (!AttachProcess(string(argv[1]), hProcess))
			return 0;
		// 이제 cout 을 어떻게 처리할 것인가가 남아있다.
	}
	
	Win32 *mWin32 = new Win32(hProcess);
	CPU *mCPU = new CPU();
	GPU *mGPU = new GPU();
	
	Win32Report *rWin32 = new Win32Report();
	CPUReport *rCPU = new CPUReport();
	GPUReport *rGPU = new GPUReport();

	mWin32->initialize();
	mCPU->initialize();
	mGPU->initialize();

	mWin32->onUpdate(rWin32);
	mCPU->onUpdate(rCPU);
	mGPU->onUpdate(rGPU);

	if (doLogging) {
		ss.str(std::string());
		ss << "MILLISEC_TIMESTAMP_IN_UTC" << "," << rCPU->header() << "," << rGPU->header() << "," << rWin32->header() << "\n";
		clog << ss.str();
	}

	isFirstShow = true;

	for (int i = 0; i < 10000; i++){
		if (stopTheWorld){
			if (doLogging)
				clog.flush();
			break;
		}

		Sleep(500);
		mWin32->onUpdate(rWin32);
		mCPU->onUpdate(rCPU);
		mGPU->onUpdate(rGPU);
		Show(rCPU, rGPU, rWin32);
		if (doLogging) {
			ss.str(std::string());
			ss << "  " << TimeStamp() << "," << rCPU << "," << rGPU << "," << rWin32 << "\n";
			clog << ss.str();
		}
	}

	delete rWin32;
	delete rCPU;
	delete rGPU;

	delete mWin32;
	delete mCPU;
	delete mGPU;

	//_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
	//_CrtDumpMemoryLeaks();
	return 1;
}