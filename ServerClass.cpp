#include "pch.h"

#include "ServerClass.h"

ServerClass::ServerClass()
{}

ServerClass::~ServerClass()
{}

HRESULT ServerClass::Initialize()
{
	TiXmlDocument config;
	if (!loadConfig(&config))
	{
		return E_FAIL;
	}

#if _DEBUG

	HRESULT hr = WinSock::GetInstance()->Initialize();
	if (FAILED(hr))
	{
		puts("WinSock Initialize Failed");
		//SLog(L"! LoginServer WINSOCK Init Failed");
	}

	hr = Clock::GetInstance()->Initialize();
	if (FAILED(hr))
	{
		puts("Clock Initialize Failed");
		//SLog(L"! LoginServer CLOCK Init Failed");
	}

	hr = SessionManager::GetInstance()->Initialize();
	if (FAILED(hr))
	{
		puts("SessionManager Initialize Failed");
		//SLog(L"! LoginServer SESSIONMANAGER Init Failed");
	}

	hr = LockManager::GetInstance()->Initialize();
	if (FAILED(hr))
	{
		puts("LockManager Initialize Failed");
		//SLog(L"! LoginServer LOCKMANAGER Init Failed");
	}

	hr = ThreadManager::GetInstance()->Initialize();
	if (FAILED(hr))
	{
		puts("ThreadManager Initialize Failed");
		//SLog(L"! LoginServer THREADMANAGER Init Failed");
	}

	hr = Monitoring::GetInstance()->Initialize();
	if (FAILED(hr))
	{
		puts("Monitoring Initialize Failed");
		//SLog(L"! LoginServer THREADMANAGER Init Failed");
	}

	//TaskManager::GetInstance()->Initialize(&config);

    puts("### IOCPServer Singleton Set ###\n");
	//SLog(L"### LoginServer Singleton set ###");

	return hr;

#else
	WinSock::GetInstance()->Initialize();
	Clock::GetInstance()->Initialize();
	SessionManager::GetInstance()->Initialize();
	LockManager::GetInstance()->Initialize();
	ThreadManager::GetInstance()->Initialize();
	TaskManager::GetInstance()->Initialize();

	return S_OK;
#endif
}

void ServerClass::release()
{
	//TaskManager::GetInstance()->release(); // 삭제
	ThreadManager::GetInstance()->Release();
	LockManager::GetInstance()->release();
	SessionManager::GetInstance()->release();
	Clock::GetInstance()->release();
	WinSock::GetInstance()->release();

	SLog(L"### IOCPServer Singleton release ###");
}

//------------------------------------------------------------------------
// 서버 쓰레드
//------------------------------------------------------------------------
void ServerClass::ServerProcess()
{
	// Smart Pointer는 참조 갯수가 0이면 자동으로 동적 메모리가 해제됨 
	std::shared_ptr<Server> server(new IOCPServer(new PacketProcess));

	//SystemReport* systemReport = new SystemReport;

	//const int MONITOR_REPORTING_SEC = 1;
	//TaskManager::GetInstance()->add(systemReport, MONITOR_REPORTING_SEC, 0);

	if (!server->Update())
	{
		SLog(L"! error: IOCPServer start fail");
		return;
	}
}

void ServerClass::Update()
{
	std::shared_ptr<Thread> createThread(new Thread(new std::thread(&ServerClass::ServerProcess, this), L"Server"));
}


// ==========================================================================
// shared_ptr<>은 특정 자원을 몇 개의 객체에서 가르키는지를
// 추적한 다음에, 그 수가 0이 되어야만 해제를 시켜주는 방식의 포인터이다
// 즉, 몇개의 shared_ptr들이 한 객체를 가르키는지 알아야 하는데
// 이게 참조 개수(reference count)라고 한다 이 참조 개수가 0이 되어야
// 가리키고 있는 객체를 해제할 수 있다
//
// <unique_ptr와 shared_ptr 차이점>
// unique_ptr로 한 객체를 가르키면 다른 unique_ptr로 이 객체를 가르키지 못한다
// 반대로, shared_ptr은 여러 shared_ptr이 한 객체를 가르킬 수 있다
// ==========================================================================
