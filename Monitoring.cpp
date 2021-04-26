#define _CRT_SECURE_NO_WARNINGS
#include "pch.h"

#include "Monitoring.h"

Monitoring::Monitoring()
{}

Monitoring::~Monitoring()
{}

Monitoring* Monitoring::GetInstance()
{
	static Monitoring monitoringManager;

	return &monitoringManager;
}

HRESULT Monitoring::Initialize()
{
	SYSTEM_INFO sysInfo;
	FILETIME ftime;
	FILETIME fsys;
	FILETIME fuser;

	GetSystemInfo(&sysInfo);
	_numProcessors = sysInfo.dwNumberOfProcessors;

	GetSystemTimeAsFileTime(&ftime);
	memcpy(&_lastCPU, &ftime, sizeof(FILETIME));

	_self = GetCurrentProcess();
	GetProcessTimes(_self, &ftime, &ftime, &fsys, &fuser);
	memcpy(&_lastSystemCPU, &fsys, sizeof(FILETIME));
	memcpy(&_lastUserCPU, &fuser, sizeof(FILETIME));

	// 모니터링 쓰레드 생성
	_thread = MAKE_THREAD(Monitoring, Update);

	return S_OK;
}

void Monitoring::Update()
{
	_startTime = (float)timeGetTime() * 0.001f;
	while (1)
	{
		//_startTime = (float)timeGetTime() * 0.001f;
		float nowTime = (float)timeGetTime() * 0.001f;
		if (nowTime - _startTime >= 0.1f)
		{
			this->ProcessCPUUse();
			this->ProcessMemoryUse();
		}
		_startTime = nowTime;
		/*this->ProcessCPUUse();
		this->ProcessMemoryUse();*/
		//this->PhysyicMemoryUse();



		//Sleep(1000);
	}
}

//-----------------------------------------------------------------------
// 현재 프로세스가 사용하고 있는 CPU
//-----------------------------------------------------------------------
double Monitoring::ProcessCPUUse()
{
	FILETIME ftime;
	FILETIME fsys;
	FILETIME fuser;
	ULARGE_INTEGER now;
	ULARGE_INTEGER sys;
	ULARGE_INTEGER user;
	double percent;

	time_t t = time(NULL);
	struct tm tm = *localtime(&t);

	GetSystemTimeAsFileTime(&ftime);
	memcpy(&now, &ftime, sizeof(FILETIME));

	GetProcessTimes(_self, &ftime, &ftime, &fsys, &fuser);
	memcpy(&sys, &fsys, sizeof(FILETIME));
	memcpy(&user, &fuser, sizeof(FILETIME));
	percent = (double)((sys.QuadPart - _lastSystemCPU.QuadPart) + (user.QuadPart - _lastUserCPU.QuadPart));
	percent /= (now.QuadPart - _lastCPU.QuadPart);
	percent /= _numProcessors;
	percent = percent * 100;

	//printf_s("[%d년-%d월-%d일 %d시:%d분:%d초] Usage > CPU :  %.2lf %%   /  ", tm.tm_year + 1900, tm.tm_mon + 1, 
	//	tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, percent);// *100);

	return min(0, max(percent, 100));
}

//--------------------------------------------------------------
// 현재 사용중인 메모리 (MB)
//--------------------------------------------------------------
size_t Monitoring::ProcessMemoryUse()
{
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
	
	//printf_s("Memory :  %u MB\n", (size_t)pmc.WorkingSetSize / 1000000);

	return (size_t)pmc.WorkingSetSize;
}


size_t Monitoring::PhysyicMemoryUse()
{
	MEMORYSTATUSEX memoryInfo;
	memoryInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memoryInfo);

	//printf_s("%u\n", (size_t)memoryInfo.ullTotalPhys - memoryInfo.ullAvailPhys);
	//printf_s("%u MB\n", (size_t)(memoryInfo.ullTotalPageFile - memoryInfo.ullAvailPageFile) / 1000000);
	return (size_t)memoryInfo.ullTotalPhys - memoryInfo.ullAvailPhys;
}