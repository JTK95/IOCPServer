#include "pch.h"

#include "ContentsProcess.h"

ContentsProcess::ContentsProcess()
	:_packageQueue(nullptr)
{
	TiXmlDocument config;
	if (!loadConfig(&config))
	{
		return;
	}

	TiXmlElement* root = config.FirstChildElement("App")->FirstChildElement("Contents");
	if (!root)
	{
		SErrLog(L"* not exist process setting");
		return;
	}

	TiXmlElement* elem = root->FirstChildElement("ThreadCount");

	int processCount = 0;

	sscanf_s(elem->GetText(), "%d", &processCount);
	if (processCount > MAX_PACKET_THREAD)
	{
		SErrLog(L"! processThread limit[%d], but config setting [%d]", MAX_PACKET_THREAD, processCount);
		return;
	}

	// ��Ű�� ť ����
	_packageQueue = new ThreadJobQueue<Package*>((WCHAR*)L"ContentsProcessQueue");

	// ������ ����
	for (int i = 0; i < processCount; ++i)
	{
		_threadPool[i] = MAKE_THREAD(ContentsProcess, Process);
	}

	this->RegistDefaultPacketFunc();
}

ContentsProcess::~ContentsProcess()
{
	_runFuncTable.clear();

	delete _packageQueue;

	for (auto thread : _threadPool)
	{
		delete thread;
	}
}

//----------------------------------------------------------------------------
// ��Ű�� ť�� ���
//----------------------------------------------------------------------------
void ContentsProcess::PutPackage(Package* package)
{
	_packageQueue->push(package);
}

//----------------------------------------------------------------------------
// �⺻ ��Ŷ �Լ� ���
//----------------------------------------------------------------------------
void ContentsProcess::RegistDefaultPacketFunc()
{
	_runFuncTable.insert(std::make_pair(PacketType::E_C_REQ_EXIT, &ContentsProcess::C_REQ_EXIT));
}

//----------------------------------------------------------------------------
// ��Ű���� ��Ŷ �Լ��� ó��
//----------------------------------------------------------------------------
void ContentsProcess::Update(Package* package)
{
	PacketType packetType = package->_packet->type();

	auto iter = _runFuncTable.find(packetType);
	if (iter == _runFuncTable.end())
	{
		printf_s("ContentsProcess::Update() �߸��� ��Ŷ�Դϴ�. packet type[%d]\n", packetType);
		//SLog(L"! invaild packet runFunction. packet type[%d]", packetType);
		package->_session->OnClose();

		return;
	}

	RunFunc runFunction = iter->second;

	//SLog(L"*** [%d] packet update ***", packetType);
	runFunction(package->_session, package->_packet);
}

//---------------------------------------------------------------------------
// ��Ű�� ó�� �� Update() ȣ��
//---------------------------------------------------------------------------
void ContentsProcess::Execute()
{
	Package* package = nullptr;

	if (!_packageQueue->pop(package))
	{
		return;
	}

	this->Update(package);

	delete package;
}

//---------------------------------------------------------------------------
// ������ �����ִ� �Լ�
//---------------------------------------------------------------------------
void ContentsProcess::Process()
{
	// shutdown ��û�� ������ ���� ���� ����
	while (!_shutdown)
	{
		this->Execute();

		// ��ǻ� �ּҴ����� 15ms ~ 16ms ������ ������ ����
		Sleep(15); 
	}
}

//---------------------------------------------------------------------------
// �⺻ ��Ŷ ��� ����
//---------------------------------------------------------------------------
void ContentsProcess::C_REQ_EXIT(Session* session, Packet* rowPacket)
{
	PK_C_REQ_EXIT* packet = (PK_C_REQ_EXIT*)rowPacket;

	User* user = UserManager::GetInstance()->Find(packet->_uid);
	if (user == nullptr)
	{
		printf_s("C_REQ_EXIT user is not exist userAccountId : %llu  \n", user->GetUID());

		delete packet;
		return;
	}

	for (auto& iter : UserManager::GetInstance()->GetUserMap())
	{
		User* otherUser = iter.second;
		if (otherUser == nullptr)
		{
			printf_s("C_REQ_EXIT otherUser is not exist userAccountId : %llu  \n", otherUser->GetUID());

			return;
		}

		if (user == otherUser)
		{
			continue;
		}

		PK_S_ANS_EXIT mySessionInfo;
		mySessionInfo._uid = user->_uid;
		otherUser->GetSession()->SendPacket(&mySessionInfo);
	}

	// ���� ����
	UserManager::GetInstance()->PopUser(user->_uid);

	// ���� ����
	SessionManager::GetInstance()->CloseSession(user->GetSession());

	// �����Ҵ� ����
	delete user;

	return;
}