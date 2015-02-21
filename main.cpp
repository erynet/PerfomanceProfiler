#include "common.h"

#include "cpu.h"
#include "gpu.h"
#include "lpc.h"
#include "win32.h"
#include "d3d.h"

//#include "visitor.h"
using namespace std;




int main(int argc, char **argv){
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	HANDLE hProcess;
	std::ios_base::sync_with_stdio(false);
	
	if (argc == 1){

	}
	else if (argc == 2){

	}
	else if (argc >= 3){

	}

	Win32 *mWin32 = new Win32();
	CPU *mCPU = new CPU();
	GPU *mGPU = new GPU();
	
	Win32Report * rWin32 = new Win32Report();
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
	ss << rCPU->header() << "," << rGPU->header() << "\n";
	cout << ss.str();

	for (int i = 0; i < 100; i++){
		Sleep(100);
		mCPU->onUpdate(rCPU);
		mGPU->onUpdate(rGPU);
		ss.str(std::string());
		ss << rCPU << "," << rGPU << "\n";
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
	return 0;
}