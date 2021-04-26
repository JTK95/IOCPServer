#pragma once

// 서버 최대 동접
#define SESSION_CAPACITY 5000


class Session;

// ===================================================================
// SessionManager Class
// 서버에는 동시에 수많은 세션이 붙고 떨어진다 
// 그리고 경우에 따라서 이들 세션에 대해 제어를 할 필요가 있다
// 서버 점검을 위해 모든 세션을 끊을 필요도 있고 공지 메시지를 전달한다
// ===================================================================

//-----------------------------------------------------
// Singleton Class
//-----------------------------------------------------
class SessionManager
{
public:
	static SessionManager* GetInstance();

	HRESULT	Initialize(int maxConnection = SESSION_CAPACITY);
	void release();

	UINT64 CreateSessionId();
	bool AddSession(Session* session);
	bool CloseSession(Session* session);

	void forceCloseSession(Session* session);
	Session* Findsession(UINT64 id);

	void runCommand(std::wstring cmdLine);
	void InitializeCommand();

	// getFunc
	std::list<Session*>& sessionList();
	UINT64 GetSeed();

	// setFunc
	void SetSeed(UINT64 seedNum);

private:
	SessionManager();
	~SessionManager();

	typedef std::list<Session*>		SessionList;

	std::list<Session*> _sessionList;
	int _sessionCount;
	
	// 동접 인원 수
	int _maxConnection;

	Lock _lock;

	// 세션 매니저 관리하는 시드
	UINT64 _sessionSeed;

	// 서버 수동 명령어
	typedef std::function<void(SessionList* sessionList, std::wstring* arg)> cmdFunc;
	std::unordered_map<std::wstring, cmdFunc> _serverCommand;
};