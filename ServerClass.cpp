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
	//TaskManager::GetInstance()->release(); // ����
	ThreadManager::GetInstance()->Release();
	LockManager::GetInstance()->release();
	SessionManager::GetInstance()->release();
	Clock::GetInstance()->release();
	WinSock::GetInstance()->release();

	SLog(L"### IOCPServer Singleton release ###");
}

//------------------------------------------------------------------------
// ���� ������
//------------------------------------------------------------------------
void ServerClass::ServerProcess()
{
	// Smart Pointer�� ���� ������ 0�̸� �ڵ����� ���� �޸𸮰� ������ 
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
// shared_ptr<>�� Ư�� �ڿ��� �� ���� ��ü���� ����Ű������
// ������ ������, �� ���� 0�� �Ǿ�߸� ������ �����ִ� ����� �������̴�
// ��, ��� shared_ptr���� �� ��ü�� ����Ű���� �˾ƾ� �ϴµ�
// �̰� ���� ����(reference count)��� �Ѵ� �� ���� ������ 0�� �Ǿ��
// ����Ű�� �ִ� ��ü�� ������ �� �ִ�
//
// <unique_ptr�� shared_ptr ������>
// unique_ptr�� �� ��ü�� ����Ű�� �ٸ� unique_ptr�� �� ��ü�� ����Ű�� ���Ѵ�
// �ݴ��, shared_ptr�� ���� shared_ptr�� �� ��ü�� ����ų �� �ִ�
// ==========================================================================
