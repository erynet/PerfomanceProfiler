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

wchar_t* CharToWChar(const char* pstrSrc){
	size_t nLen = strlen(pstrSrc) + 1;
	wchar_t* pwstr = (LPWSTR)malloc(sizeof(wchar_t)* nLen);
	mbstowcs(pwstr, pstrSrc, nLen);
	return pwstr;
}


int attachProcess(string Image, HANDLE& hProcess){
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
		//cout << GetLastError() << endl;
		// code 5 : Access is Denied
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
	ofstream log;

	stringstream ss;

	ios_base::sync_with_stdio(false);
	//redirect cerr, wcerr
	ofstream error("pf_info.txt");
	wofstream werror("pf_error.txt");
	streambuf *errbuf = std::cerr.rdbuf(error.rdbuf());
	wstreambuf *werrbuf = std::wcerr.rdbuf(werror.rdbuf());
	
	cu.Reset();
	
	if (argc == 1){
		cout << "USAGE : pf.exe target.exe\n";
		cout << "        pf.exe target.exe output.csv";
		return 0;
	}
	else if (argc == 2){
		if (!attachProcess(string(argv[1]), hProcess))
			return 0;
	}
	else if (argc >= 3){
		doLogging = true;
		log.open(argv[2]);
		clog.rdbuf(log.rdbuf());
		
		if (!attachProcess(string(argv[1]), hProcess))
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
		ss << rCPU->header() << "," << rGPU->header() << "," << rWin32->header() << "\n";
		clog << ss.str();
	}

	for (int i = 0; i < 100000; i++){
		Sleep(500);
		mWin32->onUpdate(rWin32);
		mCPU->onUpdate(rCPU);
		mGPU->onUpdate(rGPU);
		if (doLogging) {
			ss.str(std::string());
			ss << rCPU << "," << rGPU << "," << rWin32;
			clog << ss.str() << endl;
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