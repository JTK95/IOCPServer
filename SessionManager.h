#pragma once

// ���� �ִ� ����
#define SESSION_CAPACITY 5000


class Session;

// ===================================================================
// SessionManager Class
// �������� ���ÿ� ������ ������ �ٰ� �������� 
// �׸��� ��쿡 ���� �̵� ���ǿ� ���� ��� �� �ʿ䰡 �ִ�
// ���� ������ ���� ��� ������ ���� �ʿ䵵 �ְ� ���� �޽����� �����Ѵ�
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
	
	// ���� �ο� ��
	int _maxConnection;

	Lock _lock;

	// ���� �Ŵ��� �����ϴ� �õ�
	UINT64 _sessionSeed;

	// ���� ���� ��ɾ�
	typedef std::function<void(SessionList* sessionList, std::wstring* arg)> cmdFunc;
	std::unordered_map<std::wstring, cmdFunc> _serverCommand;
};