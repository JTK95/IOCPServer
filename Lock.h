#pragma once

#define __W(x)              L##x
#define _W(x)               __W(x)

class Lock
{
public:
	Lock(const WCHAR* name);
	~Lock();

	void lock(LPCWSTR fileName, int lineNo);
	void unlock();

	// setFunc
	void setThreadId(std::thread::id id);

	// getFunc
	const WCHAR* name();
	size_t lockId() const;
	std::recursive_mutex& mutex();
	std::thread::id threadId() const;

	size_t& numbeOfThreadUsing();
	
	void increaseCountNumberOfThreadUsing();
	void decreaseCountNumberOfThreadUsing();

private:
	std::recursive_mutex _mutex;
	std::wstring _name;
	size_t _lockId;
	std::thread::id _threadId;

	std::wstring _lockingFile;
	int _lockingLine;

	size_t	_threadCount;
};

class LockSafeScope
{
public:
	LockSafeScope(Lock* lock, LPCWSTR fileName, int lineNo);
	~LockSafeScope();

private:
	Lock* _lock;
};

#define SAFE_LOCK(lock)  LockSafeScope __lockSafe(&lock, _W(__FILE__), __LINE__);

//----------------------------------------------------------------
// 데드락 탐지를 위한 Singleton Class
//----------------------------------------------------------------
class LockManager
{
public:
	static LockManager* GetInstance();

	HRESULT Initialize();
	void release();

	Lock* SearchLockCycle(Lock* newLcok);
	Lock* CheckDeadLock(Lock* newLock);

	size_t GeneralId();
	
private:
	LockManager();
	~LockManager();

	size_t _idSeed;
};