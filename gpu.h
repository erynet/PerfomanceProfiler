#pragma once
#include "common.h"
#include "nvapi/nvapi.h"

#define NVAPI_MAX_PHYSICAL_GPUS   64
#define NVAPI_MAX_USAGES_PER_GPU  34
#define NVAPI_MAX_THERMAL_SENSORS_PER_GPU 3

//class GPUConfig : public IConfig{
//
//};

class GPUInfo : public IInfo{
public:
	string fullName;

	unsigned int coreCount;
	string systemType;
	string busType;

	unsigned int biosRevision;
	unsigned int biosOEMRevision;

	unsigned int baseFreqGraphics;
	unsigned int baseFreqMemory;
	unsigned int baseFreqProcessor;

	unsigned int currentFreqGraphics;
	unsigned int currentFreqMemory;
	unsigned int currentFreqProcessor;

	unsigned int boostFreqGraphics;
	unsigned int boostFreqMemory;
	unsigned int boostFreqProcessor;

	void print() {};

};

class GPUReport : public IReport{
private:
	vector<Token*> t;
	void update();
public:
	unsigned int usage;

	unsigned int currentThermalGpu;
	unsigned int currentThermalMemory;
	unsigned int currentThermalPowerSupply;

	unsigned int fanRPM;

	unsigned int currentFreqGraphics;
	unsigned int currentFreqMemory;
	unsigned int currentFreqProcessor;

	string reasonPerfDecrease;

	GPUReport();

	void reset() {};

	string header();
	string csv();

	friend ostream& operator<<(ostream& out, GPUReport& p);
};

ostream& operator<<(ostream& os, GPUReport& p){
	os << p.csv();
	return os;
}

void GPUReport::update(){
	t.clear();
	t.push_back(new Token("Usage_GPU", "% 9d", 10));
	t.push_back(new Token("Temp_GPU", "% 8d", 9));
	t.push_back(new Token("RPM_GPU_Fan", "% 11d", 12));
	t.push_back(new Token("Reason_Perf_Decrease_GPU", "% 24s", 25));

	t[0]->set(usage);
	t[1]->set(currentThermalGpu);
	t[2]->set(fanRPM);
	t[3]->set(reasonPerfDecrease);

//	for (int i = 0; i < t.size(); i++)
//		cout << t[i]->type << " ";
//	cout << endl;
}

GPUReport::GPUReport(){
//	t.push_back(new Token("Usage_GPU", "% 9d", 10));
//	t.push_back(new Token("Temp_GPU", "% 8d", 9));
//	t.push_back(new Token("RPM_GPU_Fan", "% 11d", 12));
//	t.push_back(new Token("Reason_Perf_Decrease", "% 20d", 21));
}

string GPUReport::header(){
	stringstream ss;
	this->update();

	for (int i = 0; i < t.size(); i++){
		ss << t[i]->h();
		if ((i + 1) != t.size())
			ss << ",";
	}
	return ss.str();
}

string GPUReport::csv(){
	stringstream ss;
	this->update();

	for (int i = 0; i < t.size(); i++){
		ss << t[i]->e();
		if ((i + 1) != t.size())
			ss << ",";
	}
	return ss.str();
}


typedef int *(*NvAPI_QueryInterface_t)(unsigned int offset);
typedef NvAPI_Status(*NvAPI_Initialize_t)();

typedef NvAPI_Status(*NvAPI_EnumPhysicalGPUs_t)(NvPhysicalGpuHandle nvGPUHandle[NVAPI_MAX_PHYSICAL_GPUS], NvU32 *pGpuCount);

typedef NvAPI_Status(*NvAPI_GPU_GetUsages_t)(NvPhysicalGpuHandle hPhysicalGpu, NvU32 *usages);
typedef NvAPI_Status(*NvAPI_GPU_GetThermalSettings_t)(NvPhysicalGpuHandle hPhysicalGpu, NvU32 sensorIndex, NV_GPU_THERMAL_SETTINGS *pThermalSettings);
typedef NvAPI_Status(*NvAPI_GPU_GetFullName_t)(NvPhysicalGpuHandle hPhysicalGpu, NvAPI_ShortString szName);
typedef NvAPI_Status(*NvAPI_GPU_GetGpuCoreCount_t)(NvPhysicalGpuHandle hPhysicalGpu, NvU32 *pCount);
typedef NvAPI_Status(*NvAPI_GPU_GetSystemType_t)(NvPhysicalGpuHandle hPhysicalGpu, NV_SYSTEM_TYPE *pSystemType);
typedef NvAPI_Status(*NvAPI_GPU_GetBusType_t)(NvPhysicalGpuHandle hPhysicalGpu, NV_GPU_BUS_TYPE *pBusType);

typedef NvAPI_Status(*NvAPI_GPU_GetVbiosRevision_t)(NvPhysicalGpuHandle hPhysicalGpu, NvU32 *pBiosRevision);
typedef NvAPI_Status(*NvAPI_GPU_GetVbiosOEMRevision_t)(NvPhysicalGpuHandle hPhysicalGpu, NvU32 *pBiosRevision);

typedef NvAPI_Status(*NvAPI_GPU_GetAllClockFrequencies_t)(NvPhysicalGpuHandle hPhysicalGPU, NV_GPU_CLOCK_FREQUENCIES *pClkFreqs);

typedef NvAPI_Status(*NvAPI_GPU_GetTachReading_t)(NvPhysicalGpuHandle hPhysicalGPU, NvU32 *pValue);
//typedef NvAPI_Status(*NvAPI_GPU_GetAllClocks_t)(NvPhysicalGpuHandle hPhysicalGPU, NvClocks );
typedef NvAPI_Status(*NvAPI_GPU_GetPerfDecreaseInfo_t)(NvPhysicalGpuHandle hPhysicalGpu, NvU32 *pPerfDecrInfo);

class GPU : public ISensor{
	//MAKE_SINGLETON(GPU);
private:
	NvAPI_Status NvStatus;

	NvPhysicalGpuHandle nvGPUHandle[NVAPI_MAX_PHYSICAL_GPUS];
	NvU32 pGpuCount;

	NV_GPU_THERMAL_SETTINGS a;

	// nvapi64.dll internal function pointers
	NvAPI_QueryInterface_t				NvAPI_QueryInterface;
	NvAPI_Initialize_t					NvAPI_Initialize;
	NvAPI_EnumPhysicalGPUs_t			NvAPI_EnumPhysicalGPUs;
	NvAPI_GPU_GetUsages_t				NvAPI_GPU_GetUsages;
	NvAPI_GPU_GetThermalSettings_t		NvAPI_GPU_GetThermalSettings;
	NvAPI_GPU_GetFullName_t				NvAPI_GPU_GetFullName;
	NvAPI_GPU_GetGpuCoreCount_t			NvAPI_GPU_GetGpuCoreCount;
	NvAPI_GPU_GetSystemType_t			NvAPI_GPU_GetSystemType;
	NvAPI_GPU_GetBusType_t				NvAPI_GPU_GetBusType;
	NvAPI_GPU_GetVbiosRevision_t		NvAPI_GPU_GetVbiosRevision;
	NvAPI_GPU_GetVbiosOEMRevision_t		NvAPI_GPU_GetVbiosOEMRevision;
	NvAPI_GPU_GetAllClockFrequencies_t	NvAPI_GPU_GetAllClockFrequencies;
	NvAPI_GPU_GetTachReading_t			NvAPI_GPU_GetTachReading;
	NvAPI_GPU_GetPerfDecreaseInfo_t		NvAPI_GPU_GetPerfDecreaseInfo;

public:
	~GPU();
	int initialize();
	void getInfo(IInfo *p);
	void printInfo();
	void onUpdate(IReport *p);
	string getHeaderString();
	string getUpdateString();
};

GPU::~GPU(){
}


int GPU::initialize(){
	NvAPI_QueryInterface = NULL;
	NvAPI_Initialize = NULL;
	NvAPI_EnumPhysicalGPUs = NULL;
	NvAPI_GPU_GetUsages = NULL;
	NvAPI_GPU_GetThermalSettings = NULL;
	NvAPI_GPU_GetFullName = NULL;
	NvAPI_GPU_GetGpuCoreCount = NULL;
	NvAPI_GPU_GetSystemType = NULL;
	NvAPI_GPU_GetBusType = NULL;
	NvAPI_GPU_GetVbiosRevision = NULL;
	NvAPI_GPU_GetVbiosOEMRevision = NULL;
	NvAPI_GPU_GetAllClockFrequencies = NULL;
	NvAPI_GPU_GetTachReading = NULL;
	NvAPI_GPU_GetPerfDecreaseInfo = NULL;


	HMODULE hmod = LoadLibraryA("nvapi64.dll");
	if (hmod == NULL){
		std::cerr << "GetLastError() : " << GetLastError() << std::endl;
		std::cerr << "Couldn't find nvapi64.dll" << std::endl;
		return 0;
	}

	// nvapi_QueryInterface is a function used to retrieve other internal functions in nvapi.dll
	NvAPI_QueryInterface = (NvAPI_QueryInterface_t)GetProcAddress(hmod, "nvapi_QueryInterface");


	// some useful internal functions that aren't exported by nvapi.dll
	NvAPI_Initialize = (NvAPI_Initialize_t)(*NvAPI_QueryInterface)(0x0150E828);
#ifdef __DEBUG
	if (NvAPI_Initialize == NULL)
		cerr << "GPU::initialize() / " << "NvAPI_Initialize == NULL;" << endl;
#endif
	NvAPI_EnumPhysicalGPUs = (NvAPI_EnumPhysicalGPUs_t)(*NvAPI_QueryInterface)(0xE5AC921F);
#ifdef __DEBUG
	if (NvAPI_Initialize == NULL)
		cerr << "GPU::initialize() / " << "NvAPI_EnumPhysicalGPUs == NULL;" << endl;
#endif
	NvAPI_GPU_GetUsages = (NvAPI_GPU_GetUsages_t)(*NvAPI_QueryInterface)(0x189A1FDF);
#ifdef __DEBUG
	if (NvAPI_Initialize == NULL)
		cerr << "GPU::initialize() / " << "NvAPI_GPU_GetUsages == NULL;" << endl;
#endif
	NvAPI_GPU_GetThermalSettings = (NvAPI_GPU_GetThermalSettings_t)(*NvAPI_QueryInterface)(0xE3640A56);
#ifdef __DEBUG
	if (NvAPI_Initialize == NULL)
		cerr << "GPU::initialize() / " << "NvAPI_GPU_GetThermalSettings == NULL;" << endl;
#endif
	NvAPI_GPU_GetFullName = (NvAPI_GPU_GetFullName_t)(*NvAPI_QueryInterface)(0xCEEE8E9F);
#ifdef __DEBUG
	if (NvAPI_Initialize == NULL)
		cerr << "GPU::initialize() / " << "NvAPI_GPU_GetFullName == NULL;" << endl;
#endif
	NvAPI_GPU_GetGpuCoreCount = (NvAPI_GPU_GetGpuCoreCount_t)(*NvAPI_QueryInterface)(0xC7026A87);
#ifdef __DEBUG
	if (NvAPI_Initialize == NULL)
		cerr << "GPU::initialize() / " << "NvAPI_GPU_GetGpuCoreCount == NULL;" << endl;
#endif
	NvAPI_GPU_GetSystemType = (NvAPI_GPU_GetSystemType_t)(*NvAPI_QueryInterface)(0xBAAABFCC);
#ifdef __DEBUG
	if (NvAPI_Initialize == NULL)
		cerr << "GPU::initialize() / " << "NvAPI_GPU_GetSystemType == NULL;" << endl;
#endif
	NvAPI_GPU_GetBusType = (NvAPI_GPU_GetBusType_t)(*NvAPI_QueryInterface)(0x1BB18724);
#ifdef __DEBUG
	if (NvAPI_Initialize == NULL)
		cerr << "GPU::initialize() / " << "NvAPI_GPU_GetBusType == NULL;" << endl;
#endif
	NvAPI_GPU_GetVbiosRevision = (NvAPI_GPU_GetVbiosRevision_t)(*NvAPI_QueryInterface)(0xACC3DA0A);
#ifdef __DEBUG
	if (NvAPI_Initialize == NULL)
		cerr << "GPU::initialize() / " << "NvAPI_GPU_GetVbiosRevision == NULL;" << endl;
#endif
	NvAPI_GPU_GetVbiosOEMRevision = (NvAPI_GPU_GetVbiosOEMRevision_t)(*NvAPI_QueryInterface)(0x2D43FB31);
#ifdef __DEBUG
	if (NvAPI_Initialize == NULL)
		cerr << "GPU::initialize() / " << "NvAPI_GPU_GetVbiosOEMRevision == NULL;" << endl;
#endif
	//	NvAPI_GPU_GetAllClockFrequencies =	(NvAPI_GPU_GetAllClockFrequencies_t)(*NvAPI_QueryInterface)(0x2D43FB31);
	//#ifdef __DEBUG
	//	if (NvAPI_Initialize == NULL)
	//		cerr << "GPU::initialize() / " << "NvAPI_GPU_GetAllClockFrequencies == NULL;" << endl;
	//#endif
	NvAPI_GPU_GetTachReading = (NvAPI_GPU_GetTachReading_t)(*NvAPI_QueryInterface)(0x5F608315);
#ifdef __DEBUG
	if (NvAPI_Initialize == NULL)
		cerr << "GPU::initialize() / " << "NvAPI_GPU_GetTachReading == NULL;" << endl;
#endif
	NvAPI_GPU_GetPerfDecreaseInfo = (NvAPI_GPU_GetPerfDecreaseInfo_t)(*NvAPI_QueryInterface)(0x7F7F4600);
#ifdef __DEBUG
	if (NvAPI_Initialize == NULL)
		cerr << "GPU::initialize() / " << "NvAPI_GPU_GetPerfDecreaseInfo == NULL;" << endl;
#endif

	// initialize NvAPI library, call it once before calling any other NvAPI functions
	if ((NvStatus = (*NvAPI_Initialize)()) == NVAPI_OK)
	{
		(*NvAPI_EnumPhysicalGPUs)(nvGPUHandle, &pGpuCount);
		assert(pGpuCount > 0);

		return 1;
	}
	else
	{
#ifdef __DEBUG
		switch (NvStatus){
		case NVAPI_ERROR:
			cerr << "GPU::initialize() : " << "Generic error" << endl;
			break;
		case NVAPI_LIBRARY_NOT_FOUND:
			cerr << "GPU::initialize() : " << "NVAPI support library cannot be loaded." << endl;
			break;
		case NVAPI_NO_IMPLEMENTATION:
			cerr << "GPU::initialize() : " << "not implemented in current driver installation" << endl;
			break;
		case NVAPI_NVIDIA_DEVICE_NOT_FOUND:
			cerr << "GPU::initialize() : " << "No NVIDIA display driver, or NVIDIA GPU driving a display, was found." << endl;
			break;
		}
#endif

		return 0;
	}
}

void GPU::getInfo(IInfo *p){
	GPUInfo* i = static_cast<GPUInfo*>(p);

	NvU32 nu32;
	NV_SYSTEM_TYPE nst;
	NvAPI_ShortString nss;
	NV_GPU_BUS_TYPE ngbt;

	//NvAPI_GPU_GetFullName
	NvStatus = (*NvAPI_GPU_GetFullName)(nvGPUHandle[0], nss);
#ifdef __DEBUG
	if (NvStatus != NVAPI_OK)
		cerr << "GPU::getInfo(IInfo *p) / " << "NvAPI_GPU_GetFullName(nvGPUHandle[0], nss) : " << NvStatus << endl;
#endif
	i->fullName = nss;

	//NvAPI_GPU_GetGpuCoreCount
	NvStatus = (*NvAPI_GPU_GetGpuCoreCount)(nvGPUHandle[0], &nu32);
#ifdef __DEBUG
	if (NvStatus != NVAPI_OK)
		cerr << "GPU::getInfo(IInfo *p) / " << "NvAPI_GPU_GetGpuCoreCount(nvGPUHandle[0], &nu32) : " << NvStatus << endl;
#endif
	i->coreCount = nu32;

	//NvAPI_GPU_GetSystemType
	NvStatus = (*NvAPI_GPU_GetSystemType)(nvGPUHandle[0], &nst);
#ifdef __DEBUG
	if (NvStatus != NVAPI_OK)
		cerr << "GPU::getInfo(IInfo *p) / " << "NvAPI_GPU_GetSystemType(nvGPUHandle[0], &nst) : " << NvStatus << endl;
#endif
	switch (nst)
	{
	case NV_SYSTEM_TYPE_UNKNOWN:
		i->systemType = "UNKNOWN";
		break;
	case NV_SYSTEM_TYPE_LAPTOP:
		i->systemType = "LAPTOP";
		break;
	case NV_SYSTEM_TYPE_DESKTOP:
		i->systemType = "DESKTOP";
		break;
	}

	//NvAPI_GPU_GetBusType
	NvStatus = (*NvAPI_GPU_GetBusType)(nvGPUHandle[0], &ngbt);
#ifdef __DEBUG
	if (NvStatus != NVAPI_OK)
		cerr << "GPU::getInfo(IInfo *p) / " << "NvAPI_GPU_GetBusType(nvGPUHandle[0], &ngbt) : " << NvStatus << endl;
#endif
	switch (ngbt)
	{
	case NVAPI_GPU_BUS_TYPE_UNDEFINED:
		i->busType = "UNDEFINED";
		break;
	case NVAPI_GPU_BUS_TYPE_PCI:
		i->busType = "PCI";
		break;
	case NVAPI_GPU_BUS_TYPE_AGP:
		i->busType = "AGP";
		break;
	case NVAPI_GPU_BUS_TYPE_PCI_EXPRESS:
		i->busType = "PCI_EXPRESS";
		break;
	case NVAPI_GPU_BUS_TYPE_FPCI:
		i->busType = "FPCI";
		break;
	case NVAPI_GPU_BUS_TYPE_AXI:
		i->busType = "AXI";
		break;
	}

	//NvAPI_GPU_GetVbiosRevision
	NvStatus = (*NvAPI_GPU_GetVbiosRevision)(nvGPUHandle[0], &nu32);
#ifdef __DEBUG
	if (NvStatus != NVAPI_OK)
		cerr << "GPU::getInfo(IInfo *p) / " << "NvAPI_GPU_GetVbiosRevision(nvGPUHandle[0], &nu32) : " << NvStatus << endl;
#endif
	i->biosRevision = nu32;

	//NvAPI_GPU_GetVbiosOEMRevision
	NvStatus = (*NvAPI_GPU_GetVbiosOEMRevision)(nvGPUHandle[0], &nu32);
#ifdef __DEBUG
	if (NvStatus != NVAPI_OK)
		cerr << "GPU::getInfo(IInfo *p) / " << "NvAPI_GPU_GetVbiosOEMRevision(nvGPUHandle[0], &nu32) : " << NvStatus << endl;
#endif
	i->biosRevision = nu32;
}

void GPU::printInfo(){
	GPUInfo i;
	GPU::getInfo(&i);

	cout << "Full Name : " << i.fullName << endl;
	cout << "System Type : " << i.systemType << endl;
	cout << "Core Count : " << i.coreCount << endl;
	cout << "Bus Type : " << i.busType << endl;
	cout << "Bios Revision : " << i.biosRevision << " / " << i.biosOEMRevision << endl;
}


void GPU::onUpdate(IReport* p){
	GPUReport* i = static_cast<GPUReport*>(p);
	i->reset();

	NvU32 nu32;
	NvU32 gpuUsages[NVAPI_MAX_USAGES_PER_GPU] = { 0 };
	NV_GPU_THERMAL_SETTINGS ngts;
	
	//NvAPI_GPU_GetUsages

	// gpuUsages[0] must be this value, otherwise NvAPI_GPU_GetUsages won't work
	gpuUsages[0] = (NVAPI_MAX_USAGES_PER_GPU * 4) | 0x10000;
	NvStatus = (*NvAPI_GPU_GetUsages)(nvGPUHandle[0], gpuUsages);
#ifdef __DEBUG
	if (NvStatus != NVAPI_OK)
		cerr << "GPU::onUpdate(IReport* p) / " << "NvAPI_GPU_GetUsages(nvGPUHandle[0], &nu32) : " << NvStatus << endl;
	cout << "NvAPI_GPU_GetUsages : " << gpuUsages[3] << " %" << endl;
#endif
	i->usage = gpuUsages[3];

	//NvAPI_GPU_GetThermalSettings
	ngts.version = NV_GPU_THERMAL_SETTINGS_VER_2;
	ngts.count = 0;
	ngts.sensor[0].controller = NVAPI_THERMAL_CONTROLLER_UNKNOWN;
	ngts.sensor[0].target = NVAPI_THERMAL_TARGET_GPU;
	ngts.sensor[1].controller = NVAPI_THERMAL_CONTROLLER_UNKNOWN;
	ngts.sensor[1].target = NVAPI_THERMAL_TARGET_MEMORY;
	ngts.sensor[2].controller = NVAPI_THERMAL_CONTROLLER_UNKNOWN;
	ngts.sensor[2].target = NVAPI_THERMAL_TARGET_POWER_SUPPLY;
	NvStatus = (*NvAPI_GPU_GetThermalSettings)(nvGPUHandle[0], NVAPI_THERMAL_TARGET_ALL, &ngts);
#ifdef __DEBUG
	if (NvStatus != NVAPI_OK)
		cerr << "GPU::onUpdate(IReport* p) / " << "NvAPI_GPU_GetThermalSettings(nvGPUHandle[0], NVAPI_THERMAL_TARGET_ALL, &ngts) : " << NvStatus << endl;
	cout << "NvAPI_GPU_GetThermalSettings : " << "GPU(" << ngts.sensor[0].currentTemp << "), " << "Memory(" << ngts.sensor[1].currentTemp << "), " << "PowerSupply(" << ngts.sensor[2].currentTemp << ")" << endl;
#endif
	i->currentThermalGpu = ngts.sensor[0].currentTemp;
	i->currentThermalMemory = ngts.sensor[1].currentTemp;
	i->currentThermalPowerSupply = ngts.sensor[2].currentTemp;

	//NvAPI_GPU_GetTachReading
	NvStatus = (*NvAPI_GPU_GetTachReading)(nvGPUHandle[0], &nu32);
#ifdef __DEBUG
	if (NvStatus != NVAPI_OK)
		cerr << "GPU::onUpdate(IReport* p) / " << "NvAPI_GPU_GetTachReading(nvGPUHandle[0], &nu32) : " << NvStatus << endl;
	cout << "NvAPI_GPU_GetTachReading : " << nu32 << endl;
#endif
	i->fanRPM = nu32;

	//NvAPI_GPU_GetPerfDecreaseInfo
	NvStatus = (*NvAPI_GPU_GetPerfDecreaseInfo)(nvGPUHandle[0], &nu32);
#ifdef __DEBUG
	if (NvStatus != NVAPI_OK)
		cerr << "GPU::onUpdate(IReport* p) / " << "NvAPI_GPU_GetPerfDecreaseInfo(nvGPUHandle[0], &nu32) : " << NvStatus << endl;
#endif
	switch (nu32)
	{
	case NV_GPU_PERF_DECREASE_NONE:
		i->reasonPerfDecrease = "NONE";
		break;
	case NV_GPU_PERF_DECREASE_REASON_THERMAL_PROTECTION:
		i->reasonPerfDecrease = "THERMAL_PROTECTION";
		break;
	case NV_GPU_PERF_DECREASE_REASON_POWER_CONTROL:
		i->reasonPerfDecrease = "POWER_CONTROL";
		break;
	case NV_GPU_PERF_DECREASE_REASON_AC_BATT:
		i->reasonPerfDecrease = "AC_BATT";
		break;
	case NV_GPU_PERF_DECREASE_REASON_API_TRIGGERED:
		i->reasonPerfDecrease = "API_TRIGGERED";
		break;
	case NV_GPU_PERF_DECREASE_REASON_INSUFFICIENT_POWER:
		i->reasonPerfDecrease = "INSUFFICIENT_POWER";
		break;
	case NV_GPU_PERF_DECREASE_REASON_UNKNOWN:
		i->reasonPerfDecrease = "UNKNOWN";
		break;
	}
#ifdef __DEBUG
	cout << "NvAPI_GPU_GetPerfDecreaseInfo : " << i->reasonPerfDecrease << endl;
#endif
}

string GPU::getHeaderString(){
	return string();
}

string GPU::getUpdateString(){
	GPUInfo i;
	getInfo(&i);

	return string();
}