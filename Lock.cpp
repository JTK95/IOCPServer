#include "pch.h"

#include "Lock.h"

Lock::Lock(const WCHAR* name)
	:_threadCount(0), _name(name)
{
	_lockId = LockManager::GetInstance()->GeneralId();

	_lockingFile.clear();
	_lockingLine = -1;
}

Lock::~Lock()
{
	_name.clear();
}

void Lock::lock(LPCWSTR fileName, int lineNo)
{
	_mutex.lock();

	_lockingFile = fileName;
	_lockingLine = lineNo;
}

void Lock::unlock()
{
	_mutex.unlock();

	_lockingFile.clear();
	_lockingLine = -1;
}

//--------------------------------------------------------------------
// setFunc
//--------------------------------------------------------------------
void Lock::setThreadId(std::thread::id id)
{
	_threadId = id;
}

//--------------------------------------------------------------------
// getFunc
//--------------------------------------------------------------------
const WCHAR* Lock::name() 
{
	return _name.c_str();
}

size_t Lock::lockId() const
{
	return _lockId;
}

std::recursive_mutex& Lock::mutex()
{
	return _mutex;
}

std::thread::id Lock::threadId() const
{
	return _threadId;
}

size_t& Lock::numbeOfThreadUsing() 
{
	return _threadCount;
}

void Lock::increaseCountNumberOfThreadUsing()
{
	++_threadCount;
}

void Lock::decreaseCountNumberOfThreadUsing()
{
	--_threadCount;
}

//-------------------------------------------------------------------
// LockSafeScope class
// �����ڿ��� ���� �ɰ�, ������ ���������� ��ü ������ ���� Ǭ��
//-------------------------------------------------------------------
LockSafeScope::LockSafeScope(Lock* lock, LPCWSTR fileName, int lineNo)
{
	if (lock == nullptr)
	{
		puts("LockSafeScope::LockSafeScope() non-lock!");

		return;
	}
	
	_lock = lock;

	Lock* deadLock = LockManager::GetInstance()->CheckDeadLock(_lock);
	if (deadLock != nullptr)
	{
		printf_s("LockSafeScope::LockSafeScope() [%s]lock and [%s]lock is dead detecting!\n", deadLock->name(), lock->name());
		
		return;
	}

	_lock->lock(fileName, lineNo);
	lock->setThreadId(GET_CURRENT_THREAD_ID());
}

LockSafeScope::~LockSafeScope()
{
	if (!_lock)
	{
		puts("~LockSafeScope() non-lock!");

		return;
	}

	_lock->unlock();
	//_lock->setThreadId(0);// �̻�����ȯ��������
}

//-------------------------------------------------------------------
// LockManager class
//-------------------------------------------------------------------
LockManager::LockManager()
	:_idSeed(0)
{}

LockManager::~LockManager()
{}

LockManager* LockManager::GetInstance()
{
	static LockManager lockManager;

	return &lockManager;
}

HRESULT LockManager::Initialize()
{
	return S_OK;
}

void LockManager::release()
{

}

//-------------------------------------------------------------------
// ���� �ڽ��� �ɸ� threadId��,
// Thread������ �ɰ��ִ� lock ��ȣ�� �ִ�.
// �̵��� ���� ���ٰ� �ڽ��� �ɷ��� lock�� ������ lock ��ȯ
// �� ����� ó��.
//-------------------------------------------------------------------
Lock* LockManager::SearchLockCycle(Lock* newLock)
{
	// lock ��û�� �� �����带 ThreadManager�� ���� �����´�.
	Thread* thread = ThreadManager::GetInstance()->at(GET_CURRENT_THREAD_ID());
	if (!thread)
	{
		puts("LockManager::SearchLockCycle() There is no thread requesting the lock");
		
		return nullptr;
	}

	// ����� ������ ���� ����
	std::vector<Lock*> trace;
	
	// ��û�� lock �ֱ�
	trace.push_back(newLock);

	// ������� �������� search
	Lock* deadLock = nullptr;
	while (true)
	{
		Lock* threadLock = thread->lock();
		if (threadLock == nullptr)
		{
			break;
		}

		if (threadLock->lockId() == trace[0]->lockId())
		{
			deadLock = threadLock;
			break;
		}

		trace.push_back(threadLock);

		thread = ThreadManager::GetInstance()->at(threadLock->threadId());
		if (!thread)
		{
			break;
		}
	}

	trace.empty();

	return deadLock;
}

Lock* LockManager::CheckDeadLock(Lock* newLock)
{
	Lock* deadLock = this->SearchLockCycle(newLock);
	if (deadLock)
	{
		return deadLock;
	}

	return nullptr;
}

size_t LockManager::GeneralId()
{
	size_t id = _idSeed++;

	return id;
}