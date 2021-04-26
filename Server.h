#pragma once

enum class SERVER_STATUS
{
	SERVER_STOP,
	SERVER_INITIALIZE,
	SERVER_READY
};


//-------------------------------------------------------------------------
// 1. Ŭ���̾�Ʈ ������ �޾� session�� ����, session�� �������ִ� ����
// 2. session���κ��� ���� ��Ŷ�� �ް�, ���� ���ο��� ó���� ��� ��Ŷ�� ����
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

