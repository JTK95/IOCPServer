#include "pch.h"

#include "IOCPServer.h"

IOCPServer::IOCPServer(ContentsProcess* contentsProcess)
	:Server(contentsProcess), _acceptThread(nullptr)
{}

IOCPServer::~IOCPServer()
{
	CloseHandle(_hIocp);
	closesocket(_listenSocket);
}

//-----------------------------------------------------------------------------
// ���� ���� ����, bind(), listen()
//-----------------------------------------------------------------------------
bool IOCPServer::CreateListenSocket()
{
	// ���� ���� ���� (�񵿱� ����)
	_listenSocket = WSASocket
	(
		AF_INET,
		SOCK_STREAM,
		NULL,
		NULL,
		0,
		WSA_FLAG_OVERLAPPED
	);
	if (_listenSocket == INVALID_SOCKET)
	{
		SErrLog(L"! listenSocket fail : %d", WSAGetLastError());

		return false;
	}

	// ���� �ּ�ü�� ����
	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons((unsigned short)_port);
	inet_pton(AF_INET, _ip, &(serverAddr.sin_addr));

	int reUseAddr = 1;

	// ���� �ɼǰ� ����
	setsockopt
	(
		_listenSocket,             // ����
		SOL_SOCKET,                // SOL_SOCKET
		SO_REUSEADDR,              // ���� �ٿ� �� �ٽ� ������ �ø� �� �ش� ������ �ٽ� OS�� ��û�ϱ� ����
		(char*)&reUseAddr,         // �������� �����ϱ� ���� ���� ������
		(int)sizeof(reUseAddr)     // reUseAddr ũ��
	);

	// bind
	int retval = ::bind(_listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (retval == SOCKET_ERROR)
	{
		SErrLog(L"! bind fail");

		return false;
	}

	// listen
	const int BACK_SOCKETS = 5;
	retval = listen(_listenSocket, BACK_SOCKETS);
	if (retval == SOCKET_ERROR)
	{
		SErrLog(L"! listen fail");

		return false;
	}

	std::array<char, 64> ip;
	inet_ntop(AF_INET, &(serverAddr.sin_addr), ip.data(), ip.size());
	printf_s("Create Listen Socket IP: %s, PORT: %d\n\n", ip.data(), _port);
	//SLog(L"* server listen socket created, ip: %S, port: %d", ip.data(), _port);

	return true;
}

//------------------------------------------------------------------------
// AcceptThread, WorkerThread ����, ��� ��ɾ� ���
//------------------------------------------------------------------------
bool IOCPServer::Update()
{
	if (_workerThreadCount > MAX_IOCP_THREAD)
	{
		SErrLog(L"! workerThread limit[%d], but config setting [%d]", MAX_IOCP_THREAD, _workerThreadCount);
		puts("workerThread Over Count\n");
		return false;
	}

	// createIOCP
	_hIocp = CreateIoCompletionPort
	(
		INVALID_HANDLE_VALUE,
		NULL,
		0,
		_workerThreadCount
	);
	if (_hIocp == nullptr)
	{
		return false;
	}

	// ���� ���� ����
	this->CreateListenSocket();

	// ������ ����
	_acceptThread = MAKE_THREAD(IOCPServer, AcceptThread);
	for (int i = 0; i < _workerThreadCount; ++i)
	{
		_workerThread[i] = MAKE_THREAD(IOCPServer, WorkerThread);
	}
	printf_s("workerThread Count : %d\n", _workerThreadCount);
	_status = SERVER_STATUS::SERVER_READY;

	while (!_shutdown)
	{
		std::wstring cmdLine;
		getline(std::wcin, cmdLine); // ���� "��� ��ɾ�" ���� ó���ϱ� ����

		printf_s("Input Key: %ws\n", cmdLine.c_str());
		//SLog(L"Input was: %s", cmdLine.c_str());
		SessionManager::GetInstance()->runCommand(cmdLine);

		Sleep(15);
	}

	return true;
}

//------------------------------------------------------------------------------
// Ŭ���̾�Ʈ ���ӽ� session �������, session�� IOCP ������
//------------------------------------------------------------------------------
void IOCPServer::OnAccept(SOCKET clientSocket, SOCKADDR_IN addrInfo)
{
	IOCPSession* session = new IOCPSession;
	if (session == nullptr)
	{
		puts("IOCPServer::OnAccept() Create Accpet Session Failed");
		//SLog(L"! accept session create fail");
		
		return;
	}

	if (!session->OnAccept(clientSocket, addrInfo))
	{
		delete session;

		return;
	}

	if (!SessionManager::GetInstance()->AddSession(session))
	{
		delete session;

		return;
	}

	session->_ioData[(int)(IO_OPERATION::IO_READ)].Clear();

	// clientSocket�� IOCP ����
	HANDLE handle = CreateIoCompletionPort
	(
		(HANDLE)clientSocket,
		_hIocp,
		(ULONG_PTR) &(*session),
		NULL
	);
	if (!handle)
	{
		delete session;
		
		return;
	}

	printf_s("IOCPServer::OnAccept() client Accpet from [%ws]\n", session->ClientAddress().c_str());
	//SLog(L"* client accpet from [%s]", session->ClientAddress().c_str());

	session->RecvStandBy();
}

SOCKET IOCPServer::listenSocket()
{
	return _listenSocket;
}

HANDLE IOCPServer::GetIocp()
{
	return _hIocp;
}

//------------------------------------------------------------------------------
// accept(), ���� ���� �� clientSocket�� iocp ����
//------------------------------------------------------------------------------
DWORD WINAPI IOCPServer::AcceptThread(LPVOID serverPtr)
{
	IOCPServer* server = (IOCPServer*)serverPtr;

	while (!_shutdown)
	{
		SOCKET clientSocket; // = INVALID_SOCKET;
		SOCKADDR_IN recvAddr;
		static int addrLen = sizeof(recvAddr);

		// accept
		clientSocket = WSAAccept
		(
			server->listenSocket(),
			(sockaddr*)& recvAddr,
			&addrLen,
			NULL,
			0
		);
		if (clientSocket == SOCKET_ERROR)
		{
			if (server->status() == SERVER_STATUS::SERVER_STOP)
			{
				puts("IOCPServer::AcceptThread() Accept Fail");
				//SLog(L"! Accept fail");
				break;
			}
		}
		
		server->OnAccept(clientSocket, recvAddr);

		if (server->status() != SERVER_STATUS::SERVER_READY)
		{
			break;
		}
	}

	return 0;
}

//------------------------------------------------------------------------------
// IOCP �̿��Ͽ� session ó����
//------------------------------------------------------------------------------
DWORD WINAPI IOCPServer::WorkerThread(LPVOID serverPtr)
{
	IOCPServer* server = (IOCPServer*)serverPtr;

	while (!_shutdown)
	{
		IoData* ioData = nullptr;
		IOCPSession* session = nullptr;
		DWORD transferSize;
		//puts("Recving...");
		//SLog(L"Recving");
		
		//----------------------------------------------------------------------------
		// �񵿱� ����� �Ϸ� ��ٸ���
		// ���⼱ ��������� ����� �Ϸ� ��Ʈ���� ��� ����
		//----------------------------------------------------------------------------
		BOOL ret = GetQueuedCompletionStatus
		(
			server->GetIocp(),           // �Ϸ� ��Ʈ�� �ڵ�
			&transferSize,               // ����� �Ϸ� ������ ����� ���۵� ������ ũ��
			(PULONG_PTR) &session,       // �Ϸ� Ű ���� �޴� ������ ���� ������
			(LPOVERLAPPED*) &ioData,     // OVERLAPPED ������ �ּҸ� �����ϴ� ������
			INFINITE                     // �Ϸ� ��Ŷ�� ǥ�õɶ����� ��ٸ��� �ð�
		);
		if (!ret)
		{
			continue;
		}
		
		if (session == nullptr)
		{
			puts("IOCPServer::WorkerThread() Socket data broken");
			//SLog(L"! socket data broken");

			return 0;
		}

		if (transferSize == 0)
		{
			printf_s("IOCPServer::WorkerThread() close by client[%llu][%ws]\n", session->id(), session->ClientAddress().c_str());
			//SLog(L"* close by client[%d][%s]", session->id(), session->ClientAddress());
			SessionManager::GetInstance()->CloseSession(session);
			continue;
		}

		switch(ioData->type())
		{
		case IO_OPERATION::IO_WRITE:
			session->OnSend((size_t)transferSize);
			continue;
		case IO_OPERATION::IO_READ:
			session->OnRecv((size_t)transferSize);

			// SRWLock�� ����Ͽ� ����ִ��� �������� lock�� �ɸ��� �ʰ���
			while (!session->IsEmpty())
			{
				//----------------------------------------------------------
				// session�� packetstorage�� ���� ��Ŷ�� ������ 
				// contentsProcess�� packetQueue�� �־��ش�
				//----------------------------------------------------------
				server->putPackage(session->PopPackage());
			}
			continue;
		case IO_OPERATION::IO_ERROR:
			printf_s("IOCPServer::WorkerThread() close by client error [%llu][%ws]\n", session->id(), session->ClientAddress().c_str());
			//SLog(L"* close by client error [%d][%s]", session->id(), session->ClientAddress().c_str());
			SessionManager::GetInstance()->CloseSession(session);
			continue;
		}
	}

	return 0;
}