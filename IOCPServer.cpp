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
// 리슨 소켓 생성, bind(), listen()
//-----------------------------------------------------------------------------
bool IOCPServer::CreateListenSocket()
{
	// 리슨 소켓 생성 (비동기 소켓)
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

	// 서버 주소체계 설정
	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons((unsigned short)_port);
	inet_pton(AF_INET, _ip, &(serverAddr.sin_addr));

	int reUseAddr = 1;

	// 소켓 옵션값 변경
	setsockopt
	(
		_listenSocket,             // 소켓
		SOL_SOCKET,                // SOL_SOCKET
		SO_REUSEADDR,              // 서버 다운 시 다시 서버를 올릴 때 해당 소켓을 다시 OS에 요청하기 위함
		(char*)&reUseAddr,         // 설정값을 저장하기 위한 버퍼 포인터
		(int)sizeof(reUseAddr)     // reUseAddr 크기
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
// AcceptThread, WorkerThread 생성, 비상 명령어 대기
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

	// 리슨 소켓 생성
	this->CreateListenSocket();

	// 쓰레드 생성
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
		getline(std::wcin, cmdLine); // 서버 "비상 명령어" 직접 처리하기 위함

		printf_s("Input Key: %ws\n", cmdLine.c_str());
		//SLog(L"Input was: %s", cmdLine.c_str());
		SessionManager::GetInstance()->runCommand(cmdLine);

		Sleep(15);
	}

	return true;
}

//------------------------------------------------------------------------------
// 클라이언트 접속시 session 만들어줌, session과 IOCP 연동함
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

	// clientSocket과 IOCP 연결
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
// accept(), 세션 생성 후 clientSocket과 iocp 연결
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
// IOCP 이용하여 session 처리함
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
		// 비동기 입출력 완료 기다리기
		// 여기선 쓰레드들이 입출력 완료 포트에서 대기 상태
		//----------------------------------------------------------------------------
		BOOL ret = GetQueuedCompletionStatus
		(
			server->GetIocp(),           // 완료 포트의 핸들
			&transferSize,               // 입출력 완료 연산의 결과로 전송된 데이터 크기
			(PULONG_PTR) &session,       // 완료 키 값을 받는 변수에 대한 포인터
			(LPOVERLAPPED*) &ioData,     // OVERLAPPED 구조의 주소를 수신하는 포인터
			INFINITE                     // 완료 패킷이 표시될때까지 기다리는 시간
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

			// SRWLock을 사용하여 비어있는지 읽을때는 lock이 걸리지 않게함
			while (!session->IsEmpty())
			{
				//----------------------------------------------------------
				// session의 packetstorage의 넣은 패킷을 꺼내서 
				// contentsProcess의 packetQueue에 넣어준다
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