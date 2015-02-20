#include "common.h"
#include "cpu.h"
#include "gpu.h"

using namespace std;

int main(int argc, char **argv){

	CPU *mCPU = new CPU();
	GPU *mGPU = new GPU();

	mCPU->initialize();
	mGPU->initialize();

	CPUReport rCPU;
	GPUReport rGPU;

	for (int i = 0; i < 1000; i++){
		Sleep(1000);
		mCPU->onUpdate(&rCPU);
		mGPU->onUpdate(&rGPU);
	}

	return 0;
}