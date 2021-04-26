#pragma once
class Thread;
//-----------------------------------------------------
// Singleton Class
// CPU / MEMORY MONITORING
//-----------------------------------------------------
class Monitoring
{
public:
	static Monitoring* GetInstance();

	HRESULT Initialize();

	double ProcessCPUUse();
	size_t ProcessMemoryUse();
	size_t PhysyicMemoryUse();

	void Update();

private:
	Monitoring();
	~Monitoring();

private:
	ULARGE_INTEGER _lastCPU;
	ULARGE_INTEGER _lastSystemCPU;
	ULARGE_INTEGER _lastUserCPU;

	int _numProcessors;

	HANDLE _self;

	Thread* _thread;

	float _startTime;
};

