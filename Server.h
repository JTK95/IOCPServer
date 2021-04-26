#pragma once

enum class SERVER_STATUS
{
	SERVER_STOP,
	SERVER_INITIALIZE,
	SERVER_READY
};


//-------------------------------------------------------------------------
// 1. 클라이언트 접속을 받아 session을 생성, session을 관리해주는 역할
// 2. session으로부터 오는 패킷을 받고, 서버 내부에서 처리된 결과 패킷을 전송
//-------------------------------------------------------------------------
class Server
{
public:
	Server(ContentsProcess* contentsProcess);
	virtual ~Server();

	//virtual void Initialize(TiXmlDocument* config);

	void putPackage(Package* package);
	
	virtual bool Update() = 0;

	SERVER_STATUS& status();

protected:
	char _ip[20];
	int _port;
	int _workerThreadCount;

	SERVER_STATUS _status;
	ContentsProcess* _contentsProcess;
};

