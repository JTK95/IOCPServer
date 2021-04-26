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
// 생성자에서 락을 걸고, 스코프 빠져나가는 객체 해제시 락을 푼다
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
	//_lock->setThreadId(0);// 이새끼반환형ㅈ같네
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
// 락은 자신이 걸린 threadId를,
// Thread에서는 걸고있는 lock 번호가 있다.
// 이들을 따라가 보다가 자신이 걸려는 lock이 나오면 lock 순환
// 즉 데드락 처리.
//-------------------------------------------------------------------
Lock* LockManager::SearchLockCycle(Lock* newLock)
{
	// lock 요청을 한 쓰레드를 ThreadManager를 통해 가져온다.
	Thread* thread = ThreadManager::GetInstance()->at(GET_CURRENT_THREAD_ID());
	if (!thread)
	{
		puts("LockManager::SearchLockCycle() There is no thread requesting the lock");
		
		return nullptr;
	}

	// 데드락 추적을 위한 벡터
	std::vector<Lock*> trace;
	
	// 요청한 lock 넣기
	trace.push_back(newLock);

	// 데드락을 루프돌며 search
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