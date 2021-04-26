#include "pch.h"

#include "SessionManager.h"

SessionManager::SessionManager()
	:_lock(L"SessionManager"), _sessionSeed(1)
{
	//this->InitializeCommand();
}

SessionManager::~SessionManager()
{
	std::vector<Session*> removeSessionVec;
	removeSessionVec.resize(_sessionList.size());
	copy(_sessionList.begin(), _sessionList.end(), removeSessionVec.begin());

	for (auto session : removeSessionVec)
	{
		session->OnClose();
	}

	_sessionList.clear();
}

SessionManager* SessionManager::GetInstance()
{
	static SessionManager sessionManager;

	return &sessionManager;
}

//-----------------------------------------------------------------------------
// 생성할 세션 5000명을 매개변수인 maxConnection의 default로 미리 대입 되어있다
// <maxConnection = 5000>
//-----------------------------------------------------------------------------
HRESULT	SessionManager::Initialize(int maxConnection)
{
	_maxConnection = maxConnection;

	return S_OK;
}

void SessionManager::release()
{
	std::vector<Session*> removeSessionVec;
	removeSessionVec.resize(_sessionList.size());
	copy(_sessionList.begin(), _sessionList.end(), removeSessionVec.begin());

	for (auto session : removeSessionVec)
	{
		session->OnClose();
	}

	_sessionList.clear();
}

//------------------------------------------------------------------------
// _sessionSeed 증가 후 리턴
//------------------------------------------------------------------------
UINT64 SessionManager::CreateSessionId()
{
	return ++_sessionSeed;
}

//------------------------------------------------------------------------
// session을 추가해줌, 동일한 세션이 존재하면 false
//------------------------------------------------------------------------
bool SessionManager::AddSession(Session* session)
{
	SAFE_LOCK(_lock);

	auto findSession = find(_sessionList.begin(), _sessionList.end(), session);
	if (findSession != _sessionList.end())
	{
		return false;
	}

	if (_sessionCount > _maxConnection)
	{
		printf_s("SessionManager::AddSession() busy session.. count[%d]\n", _sessionList);
		//SLog(L"* session so busy. count[%d]", _sessionList);

		return false;
	}

	session->setId(this->CreateSessionId());
	_sessionList.push_back(session);
	++_sessionCount;

	return true;
}

//------------------------------------------------------------------------
// session을 제거해줌, 소캣을 닫으라고 클라이언트에게 요청
//------------------------------------------------------------------------
bool SessionManager::CloseSession(Session* session)
{
	SAFE_LOCK(_lock);

	if (session == nullptr)
	{
		return false;
	}

	auto findSession = find(_sessionList.begin(), _sessionList.end(), session);
	if (findSession != _sessionList.end())
	{
		Session* deleteSession = *findSession;
		printf_s("close session [%p]\n", deleteSession->ClientAddress().c_str());
		//SLog(L"* detected close by [%s]", deleteSession->ClientAddress().c_str());
		closesocket(deleteSession->Socket());

		_sessionList.remove(deleteSession);
		--_sessionCount;

		delete deleteSession;

		return true;
	}

	return false;
}

//------------------------------------------------------------------------
// session을 제거해줌, 소켓을 강제로 닫음
//------------------------------------------------------------------------
void SessionManager::forceCloseSession(Session* session)
{
	SAFE_LOCK(_lock);
	
	if (!session)
	{
		return;
	}

	// 종료 유도. 원래는 클라이언트에서 서버 접속을 종료하도록 유도해야 한다
	LINGER linger;
	linger.l_onoff = 1; // 사용
	linger.l_linger = 0; // 대기시간, 0일시 완료 안 된 패킷 버리고 즉시 종료

	setsockopt
	(
		session->Socket(),
		SOL_SOCKET, SO_LINGER,
		(char*)&linger,
		sizeof(linger)
	);

	this->CloseSession(session);
}

//------------------------------------------------------------------------
// 찾는 세션 리턴
//------------------------------------------------------------------------
Session* SessionManager::Findsession(UINT64 id)
{
	SAFE_LOCK(_lock);

	Session* findSession = nullptr;

	for (auto session : _sessionList)
	{
		if (session->id() >= this->GetSeed())
		{
			puts("SessionManager::session() cannot find session");
			//SLog(L"can not find session");

			return nullptr;
		}

		if (session == nullptr && session->id() < this->GetSeed())
		{
			continue;
		}

		if (session->id() == id)
		{
			printf_s("SessionManager::session() find sessionID : %llu, UINT64 : %llu\n", session->id(), id);
			//SLog(L"session id : %d, UINT64 : %d", session->id(), id);
			findSession = session;
			break;
		}
	}

	return findSession;
}

//------------------------------------------------------------------------
// cmd에 따라 서버 커맨드 처리
//------------------------------------------------------------------------
void SessionManager::runCommand(std::wstring cmdLine)
{
	size_t found = cmdLine.find(L' ');
	std::wstring command;

	std::wstring arg;

	if (found != std::wstring::npos)
	{
		command = cmdLine.substr(0, found);
		arg = cmdLine.substr(found);
	}
	else
	{
		command = cmdLine;
	}

	auto iter = _serverCommand.find(command);
	if (iter == _serverCommand.end())
	{
		return;
	}

	auto cmdFunc = _serverCommand.at(command);
	if (cmdFunc)
	{
		cmdFunc(&_sessionList, &arg);
	}
}

//------------------------------------------------------------------------
// 커맨드 초기화(치트키)
//------------------------------------------------------------------------
void SessionManager::InitializeCommand()
{
	// 생략
}

std::list<Session*>& SessionManager::sessionList()
{
	return _sessionList;
}

UINT64 SessionManager::GetSeed()
{
	return _sessionSeed;
}

void SessionManager::SetSeed(UINT64 seedNum)
{
	_sessionSeed = seedNum;
}
