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

	// 패키지 큐 생성
	_packageQueue = new ThreadJobQueue<Package*>((WCHAR*)L"ContentsProcessQueue");

	// 쓰레드 생성
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
// 패키지 큐에 담기
//----------------------------------------------------------------------------
void ContentsProcess::PutPackage(Package* package)
{
	_packageQueue->push(package);
}

//----------------------------------------------------------------------------
// 기본 패킷 함수 등록
//----------------------------------------------------------------------------
void ContentsProcess::RegistDefaultPacketFunc()
{
	_runFuncTable.insert(std::make_pair(PacketType::E_C_REQ_EXIT, &ContentsProcess::C_REQ_EXIT));
}

//----------------------------------------------------------------------------
// 패키지를 패킷 함수로 처리
//----------------------------------------------------------------------------
void ContentsProcess::Update(Package* package)
{
	PacketType packetType = package->_packet->type();

	auto iter = _runFuncTable.find(packetType);
	if (iter == _runFuncTable.end())
	{
		printf_s("ContentsProcess::Update() 잘못된 패킷입니다. packet type[%d]\n", packetType);
		//SLog(L"! invaild packet runFunction. packet type[%d]", packetType);
		package->_session->OnClose();

		return;
	}

	RunFunc runFunction = iter->second;

	//SLog(L"*** [%d] packet update ***", packetType);
	runFunction(package->_session, package->_packet);
}

//---------------------------------------------------------------------------
// 패키지 처리 및 Update() 호출
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
// 쓰레드 돌려주는 함수
//---------------------------------------------------------------------------
void ContentsProcess::Process()
{
	// shutdown 요청이 있을때 까지 무한 루프
	while (!_shutdown)
	{
		this->Execute();

		// 사실상 최소단위가 15ms ~ 16ms 단위로 프레임 실행
		Sleep(15); 
	}
}

//---------------------------------------------------------------------------
// 기본 패킷 기능 구현
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

	// 유저 삭제
	UserManager::GetInstance()->PopUser(user->_uid);

	// 세션 종료
	SessionManager::GetInstance()->CloseSession(user->GetSession());

	// 동적할당 해제
	delete user;

	return;
}