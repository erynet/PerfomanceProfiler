#pragma once
#include "common.h"

#include "pcm/cpucounters.h"
#include "pcm/utils.h"


class CPUInfo : public IInfo{
public:
	string CPUBrandString;
	uint32 CPUModel;
	string CPUCodeName;
	uint32 OriginalCPUModel;

	bool haveSMT;
	uint32 ThreadsPerCore;
	uint32 PhysicalCoreCount;
	uint32 LogicalCoreCount;

	uint64 NominalFrequency;

	int32 PackageThermalSpecPower;
	int32 PackageMinimumPower;
	int32 PackageMaximumPower;
	void print()  {};
};

//class CPUReport : public IReport{
//private:
//	vector<Token*> t;
//	void update();
//public:	
//	int32 ThermalInfoOfSocket;
//	vector<int32> ThermalInfoPerCore;
//	double ConsumedJoules;
//	double C0StatePercent;
//
//	CPUReport();
//	
//	void reset();
//
//	string header();
//	string csv();
//	friend ostream& operator<<(ostream& out, CPUReport& p);
//	friend ostream& operator<<(ostream& out, CPUReport* p);
//	//friend ostream& operator<<(ostream& out, IReport& p);
//	//friend ostream& operator<<(ostream& out, IReport* p);
//};

class CPUReport : public IReport{
private:
	virtual void update();
public:
	int32 ThermalInfoOfSocket;
	vector<int32> ThermalInfoPerCore;
	double ConsumedJoules;
	double C0StatePercent;

	CPUReport() {};
	~CPUReport();

	void reset();

	string header();
	string csv();
	friend ostream& operator<<(ostream& out, CPUReport& p);
	friend ostream& operator<<(ostream& out, CPUReport* p);

	//friend ostream& operator<<(ostream& out, IReport& p);
	//friend ostream& operator<<(ostream& out, IReport* p);
};

ostream& operator<<(ostream& out, CPUReport& p){
	out << p.csv();
	return out;
}

ostream& operator<<(ostream& out, CPUReport* p){
	out << p->csv();
	return out;
}

//ostream& operator<<(ostream& out, IReport& p){
//	CPUReport *i = static_cast<CPUReport*>(&p);
//	out << i->csv();
//	return out;
//}

//ostream& operator<<(ostream& out, IReport* p){
//	CPUReport *i = static_cast<CPUReport*>(p);
//	out << i->csv();
//	return out;
//}


void CPUReport::update(){
	for (int i = 0; i < t.size(); i++){
		if (t[i] != NULL)
			delete t[i];
	}
	t.clear();

	t.push_back(new Token("Temp_Socket", "% 11d", 12));
	for (int i = 0; i < ThermalInfoPerCore.size(); i++){
		stringstream ss;
		ss << "Temp_Core" << i;
		t.push_back(new Token(ss.str(), "% 10d", 11));
	}
	t.push_back(new Token("Consumed_Energy_CPU_In_Joules", "% 29.2lf", 30));
	t.push_back(new Token("C0_State_Percent_Of_CPU", "% 23.2lf", 24));

	
	t[0]->set(ThermalInfoOfSocket);
	for (int i = 1; i < (ThermalInfoPerCore.size() + 1); i++){
		t[i]->set(ThermalInfoPerCore[i - 1]);
	}
	t[ThermalInfoPerCore.size() + 1]->set(ConsumedJoules);
	t[ThermalInfoPerCore.size() + 2]->set(C0StatePercent);

}

CPUReport::~CPUReport(){
	for (int i = 0; i < t.size(); i++){
		if (t[i] != NULL)
			delete t[i];
	}
}

void CPUReport::reset(){
	ThermalInfoPerCore.clear();
}

string CPUReport::header(){
	stringstream ss;
	this->update();

	for (int i = 0; i < t.size(); i++){
		ss << t[i]->h();
		if ((i + 1) != t.size())
			ss << ",";
	}
	return ss.str();
}

string CPUReport::csv(){
	stringstream ss;
	this->update();
	
	for (int i = 0; i < t.size(); i++){
		ss << t[i]->e();
		if ((i + 1) != t.size())
			ss << ",";
	}
	return ss.str();
}


class CPU : public ISensor{
	//MAKE_SINGLETON(CPU);
private:
	PCM *m;
	vector<CoreCounterState> cstates1, cstates2;
	vector<SocketCounterState> sktstate1, sktstate2;
	SystemCounterState sstate1, sstate2;

	vector<int32> thermalInfoPerCore;

	int32 thermalInfoOfSocket;
	double coreUtilization;
	double consumedJoules;

	string CPUBrandString;
	uint32 CPUModel;
	string CPUCodeName;
	uint32 originalCPUModel;

	bool haveSMT;
	uint32 threadsPerCore;
	uint32 logicalCoreCount;
	uint32 physicalCoreCount;

	uint64 nominalFrequency;

	uint32 thermalJunctionMax;

	int32 packageThermalSpecPower;
	int32 packageMinimumPower;
	int32 packageMaximumPower;
public:
	~CPU();
	int initialize();
	void getInfo(IInfo *p);
	void printInfo();
	void onUpdate(IReport *p);
	string getHeaderString();
	string getUpdateString();
};

CPU::~CPU(){
	delete m;
}

int CPU::initialize(){
	m = PCM::getInstance();

	CPUBrandString = m->getCPUBrandString();
	CPUModel = m->getCPUModel();
	CPUCodeName = m->getUArchCodename();
	originalCPUModel = m->getOriginalCPUModel();

	haveSMT = m->getSMT();
	threadsPerCore = m->getThreadsPerCore();
	logicalCoreCount = m->getNumCores();
	physicalCoreCount = logicalCoreCount / threadsPerCore;

	nominalFrequency = m->getNominalFrequency();

	//hardcoded
	thermalJunctionMax = 100;

	packageThermalSpecPower = m->getPackageThermalSpecPower();
	packageMinimumPower = m->getPackageMinimumPower();
	packageMaximumPower = m->getPackageMaximumPower();

	m->getAllCounterStates(sstate1, sktstate1, cstates1);
	return 1;
}

void CPU::getInfo(IInfo *p){
	CPUInfo *i = static_cast<CPUInfo*>(p);
	i->CPUBrandString = CPUBrandString;
	i->CPUModel = CPUModel;
	i->CPUCodeName = CPUCodeName;
	i->OriginalCPUModel = originalCPUModel;

	i->haveSMT = haveSMT;
	i->ThreadsPerCore = threadsPerCore;
	i->LogicalCoreCount = logicalCoreCount;
	i->PhysicalCoreCount = physicalCoreCount;

	i->NominalFrequency = nominalFrequency;

	i->PackageThermalSpecPower = packageThermalSpecPower;
	i->PackageMinimumPower = packageMinimumPower;
	i->PackageMaximumPower = packageMaximumPower;
}

void CPU::printInfo(){
	CPUInfo i;
	CPU::getInfo(&i);

	cout << "----- ----- ----- ----- -----" << endl;
	cout << "Full Name : " << i.CPUBrandString << endl;
	cout << "Model Number : " << i.CPUModel << endl;
	cout << "Codename : " << i.CPUCodeName << endl;
	cout << "Original CPU Model : " << i.OriginalCPUModel << endl;

	cout << "Threads Per Core : " << i.ThreadsPerCore << endl;
	cout << "Logical Core Count : " << i.LogicalCoreCount << endl;
	cout << "Physical Core Count : " << i.PhysicalCoreCount << endl;

	cout << "Nominal Frequency : " << i.NominalFrequency << endl;

	cout << "Package Thermal Spec Power : " << i.PackageThermalSpecPower << endl;
	cout << "Package Minimum Power : " << i.PackageMinimumPower << endl;
	cout << "Package Maximum Power : " << i.PackageMaximumPower << endl;
}

void CPU::onUpdate(IReport *p){
	CPUReport* i = static_cast<CPUReport*>(p);
	i->reset();

	m->getAllCounterStates(sstate2, sktstate2, cstates2);

	i->C0StatePercent = getCoreCStateResidency(0, sstate1, sstate2) * 100.;
	i->ConsumedJoules = getConsumedJoules(sktstate1[0], sktstate2[0]);
	i->ThermalInfoOfSocket = thermalJunctionMax - sktstate2[0].getThermalHeadroom();
	for (unsigned int j = 0; j < physicalCoreCount; j++)
		i->ThermalInfoPerCore.push_back(thermalJunctionMax - (cstates2[j * threadsPerCore].getThermalHeadroom()));
	m->getAllCounterStates(sstate1, sktstate1, cstates1);

#ifdef __DEBUG
	cout << "CoreUtilization : " << i->CoreUtilization << endl;
	cout << "ConsumedJoules : " << i->ConsumedJoules << endl;
	cout << "ThermalInfoOfSocket : " << i->ThermalInfoOfSocket << endl;
	cout << "ThermalInfoPerCore : " << endl;
	for (unsigned int j = 0; j < physicalCoreCount; j++)
		cout << "    " << i->ThermalInfoPerCore[j] << endl;
#endif
}

string CPU::getHeaderString(){ return string(); }
string CPU::getUpdateString(){ return string(); }