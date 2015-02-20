#include "common.h"

#include "cpu.h"
#include "gpu.h"
#include "lpc.h"
#include "os.h"
#include "d3d.h"

#include "visitor.h"

using namespace std;

int main(int argc, char **argv){

	CPU *mCPU = new CPU();
	GPU *mGPU = new GPU();
	CPUReport *rCPU = new CPUReport();
	GPUReport *rGPU = new GPUReport();

	stringstream ss;

	mCPU->initialize();
	mGPU->initialize();

	mCPU->onUpdate(rCPU);
	mGPU->onUpdate(rGPU);

	//cout << "a" << endl;
	ss.str(std::string());
	ss << rCPU->header() << "," << rGPU->header();
	cout << ss.str() << endl;
	//cout << "b" << endl;

	for (int i = 0; i < 1000; i++){
		Sleep(1000);
		mCPU->onUpdate(rCPU);
		mGPU->onUpdate(rGPU);
		ss.str(std::string());
		//ss << rCPU->csv() << "," << rGPU->csv();
		ss << (*rCPU) << "," << (*rGPU);
		cout << ss.str() << endl;
	}	

	delete rCPU;
	delete rGPU;

	delete mCPU;
	delete mGPU;

	return 0;
}