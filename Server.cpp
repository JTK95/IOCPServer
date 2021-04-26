#include "pch.h"

#include "Server.h"

//----------------------------------------------------------------------------
// config ���� ���� ip, port ����
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

	// ���� ����
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
	// �̷л����� workerThread ������ (CPU * 2) �̴�
	// ���� �׽�Ʈ�� �غ��� �ʾ����� �� CPU�ھ�� 8�� �̹Ƿ�
	// workerThread�� 16���� ��������.
	// ���Ŀ� �׽�Ʈ�غ��� ���� ���.
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