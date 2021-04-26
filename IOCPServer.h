#pragma once

#define MAX_IOCP_THREAD 64


class IOCPServer : public Server
{
public:
	IOCPServer(ContentsProcess* contentsProcess);
	~IOCPServer();

	bool Update();
	void OnAccept(SOCKET clientSocket, SOCKADDR_IN addrInfo);

	// getFunc
	SOCKET listenSocket();
	HANDLE GetIocp();
			
private:
	bool CreateListenSocket();
	
	static DWORD WINAPI AcceptThread(LPVOID serverPtr);
	static DWORD WINAPI WorkerThread(LPVOID serverPtr);

	SOCKET _listenSocket;
	HANDLE _hIocp;

	Thread* _acceptThread;
	std::array<Thread*, MAX_IOCP_THREAD> _workerThread;
};

