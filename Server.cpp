#include "pch.h"

#include "Server.h"

//----------------------------------------------------------------------------
// config 파일 통해 ip, port 설정
//----------------------------------------------------------------------------
Server::Server(ContentsProcess* contentsProcess)
	:_contentsProcess(contentsProcess), _status(SERVER_STATUS::SERVER_STOP)
{
	puts("\n###  Initialize Network base  ###\n");
	//SLog(L"# Initialize network base");

	TiXmlDocument config;
	if (!loadConfig(&config))
	{
		return;
	}

	// 서버 설정
	TiXmlElement* root = config.FirstChildElement("App")->FirstChildElement("Server");
	if (!root)
	{
		puts("Server::Initialize() not exist server setting");
		//SLog(L"@ not exist server setting");

		return;
	}

	TiXmlElement* elem = root->FirstChildElement("IP");
	strcpy_s(_ip, elem->GetText());

	elem = root->FirstChildElement("Port");
	sscanf_s(elem->GetText(), "%d", &_port);

	elem = root->FirstChildElement("ThreadCount");
	sscanf_s(elem->GetText(), "%d", &_workerThreadCount);

	_status = SERVER_STATUS::SERVER_INITIALIZE;
	printf_s("Server::Initialize() IO Worker Thread Count : %d\n\n", _workerThreadCount);
	//----------------------------------------------------------
	// 이론상으로 workerThread 개수가 (CPU * 2) 이다
	// 아직 테스트를 해보지 않았지만 내 CPU코어는 8개 이므로
	// workerThread를 16개를 생성하자.
	// 추후에 테스트해보고 변경 요망.
	//----------------------------------------------------------

	root = config.FirstChildElement("App");
	elem = root->FirstChildElement("Name");

	printf_s("### %s START!!! ###\n\n", elem->GetText());
	//SLog(L"### %S strat!!! ###", elem->GetText());
}

Server::~Server()
{
	shutdownServer();

	_status = SERVER_STATUS::SERVER_STOP;
	delete _contentsProcess;

	puts("\n###  End Network...  ###\n");
	//SLog(L"# End network base");
}

void Server::putPackage(Package* package)
{
	_contentsProcess->PutPackage(package);
}

SERVER_STATUS& Server::status()
{
	return _status;
}