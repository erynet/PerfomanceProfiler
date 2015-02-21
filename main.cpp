#include "common.h"

#include "cpu.h"
#include "gpu.h"
#include "lpc.h"
#include "win32.h"
#include "d3d.h"

//#include "visitor.h"
using namespace std;

wchar_t* CharToWChar(const char* pstrSrc){
	//ASSERT(pstrSrc);
	size_t nLen = strlen(pstrSrc) + 1;
	wchar_t* pwstr = (LPWSTR)malloc(sizeof(wchar_t)* nLen);
	mbstowcs(pwstr, pstrSrc, nLen);
	return pwstr;
}

int attachProcess(string Image, HANDLE& hProcess){
	HANDLE hProcessSnap;
	PROCESSENTRY32 pe32;

	//convert string to wchar_t
	//const size_t len = Image.length() + 1;
	//wchar_t wcImageName[512];
	//swprintf(wcImageName, len, L"%s", Image.c_str());

	//cout << Image << endl;
	//cout << "|" << endl;
	//cout << Image.c_str() << endl;
	//cout << "|" << endl;
	//wcout << CharToWChar(Image.c_str()) << endl;
	
	//take a snapshot of current process list
	//while (1){
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
				cout << "CATCH" << endl;
				break;
			}
		} while (Process32Next(hProcessSnap, &pe32));

		CloseHandle(hProcessSnap);

		//console io

	//}

		cout << "A" << endl;

	if (hProcess == NULL){
		cout << GetLastError() << endl;
		return 0;
	}
	else
		return 1;
}


int main(int argc, char **argv){
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	HANDLE hProcess = NULL;
	std::ios_base::sync_with_stdio(false);
	
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

	stringstream ss;

	mWin32->initialize();
	mCPU->initialize();
	mGPU->initialize();

	mWin32->onUpdate(rWin32);
	mCPU->onUpdate(rCPU);
	mGPU->onUpdate(rGPU);

	ss.str(std::string());
	ss << rCPU->header() << "," << rGPU->header() << "," << rWin32->header() << "\n";
	cout << ss.str();

	for (int i = 0; i < 1000; i++){
		Sleep(500);
		mWin32->onUpdate(rWin32);
		mCPU->onUpdate(rCPU);
		mGPU->onUpdate(rGPU);
		ss.str(std::string());
		ss << rCPU << "," << rGPU << "," << rWin32 << "\n";
		cout << ss.str();
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