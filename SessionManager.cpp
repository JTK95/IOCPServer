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
// ������ ���� 5000���� �Ű������� maxConnection�� default�� �̸� ���� �Ǿ��ִ�
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
// _sessionSeed ���� �� ����
//------------------------------------------------------------------------
UINT64 SessionManager::CreateSessionId()
{
	return ++_sessionSeed;
}

//------------------------------------------------------------------------
// session�� �߰�����, ������ ������ �����ϸ� false
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
// session�� ��������, ��Ĺ�� ������� Ŭ���̾�Ʈ���� ��û
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
// session�� ��������, ������ ������ ����
//------------------------------------------------------------------------
void SessionManager::forceCloseSession(Session* session)
{
	SAFE_LOCK(_lock);
	
	if (!session)
	{
		return;
	}

	// ���� ����. ������ Ŭ���̾�Ʈ���� ���� ������ �����ϵ��� �����ؾ� �Ѵ�
	LINGER linger;
	linger.l_onoff = 1; // ���
	linger.l_linger = 0; // ���ð�, 0�Ͻ� �Ϸ� �� �� ��Ŷ ������ ��� ����

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
// ã�� ���� ����
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
// cmd�� ���� ���� Ŀ�ǵ� ó��
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
// Ŀ�ǵ� �ʱ�ȭ(ġƮŰ)
//------------------------------------------------------------------------
void SessionManager::InitializeCommand()
{
	// ����
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
