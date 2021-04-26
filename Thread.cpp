#include "pch.h"

#include "Thread.h"

Thread::Thread(std::thread* thread, std::wstring name)
	:_thread(thread), _name(name), _id(thread->get_id()), _lock(nullptr)
{
	ThreadManager::GetInstance()->Put(this);
}

Thread::Thread(Lock* pLock, std::thread* thread, std::wstring name)
	:_name(name), _thread(thread), _id(thread->get_id())
{
	this->SetLock(pLock);
	ThreadManager::GetInstance()->Put(this);
}

Thread::~Thread()
{
	// 쓰레드 끝날때까지 대기
	if (_thread->joinable())
	{
		_thread->join();
	}

	// 그리고 삭제
	ThreadManager::GetInstance()->Remove(_id);
	
	// 메모리 해제
	delete _thread;

	//---------------------------------------------------------------------
	// 1번 쓰레드가 종료되어 소멸자에서 lock을 해제함.
	// 2번 쓰레드가 lock을 거는 중이었다.
	// 이 경우 2번 쓰레드는 오류가 날 것이다.
	// lock을 제일 마지막에 사용하는 쓰레드가 lock을 해제시켜주자
	//---------------------------------------------------------------------
	if (_lock && _lock->numbeOfThreadUsing() > 1)
	{
		_lock->decreaseCountNumberOfThreadUsing();
	}
	else
	{
		delete _lock;
	}
}

void Thread::SetLock(Lock* lock)
{
	_lock = lock;
}

std::thread::id Thread::id()
{
	return _id;
}

std::wstring& Thread::name()
{
	return _name;
}

Lock* Thread::lock()
{
	return _lock;
}

//-----------------------------------------------------------------
// Singleton class
//-----------------------------------------------------------------
ThreadManager::ThreadManager()
{}

ThreadManager::~ThreadManager()
{}

ThreadManager* ThreadManager::GetInstance()
{
	static ThreadManager threadManager;

	return &threadManager;
}

HRESULT ThreadManager::Initialize()
{
	return S_OK;
}

void ThreadManager::Put(Thread* thread)
{
	// pair로 node를 생성 후 쓰레드 풀에 삽입
	std::pair<std::thread::id, Thread*> node(thread->id(), thread);
	_threadPool.insert(node);
	printf_s("ThreadManager::Put() create thread : id[0x%x] name[%ws], pool size[%d]\n", thread->id(),
		thread->name().c_str(), _threadPool.size());
	/*SLog(L"create thread : id[0x%X] name[%s], pool size[%d]\n", thread->id(),
		thread->name().c_str(), _threadPool.size());*/
}

void ThreadManager::Remove(std::thread::id id)
{
	auto iter = _threadPool.find(id);
	if (iter == _threadPool.end())
	{
		return;
	}

	auto thread = iter->second;
	_threadPool.erase(iter);
}

void ThreadManager::Release()
{
	std::vector<Thread*> tempVector;
	for (auto thread : _threadPool)
	{
		tempVector.push_back(thread.second);
	}

	for (int i = 0; i < tempVector.size(); i++)
	{
		delete tempVector[i];
	}
}

Thread* ThreadManager::at(std::thread::id id)
{
	if (_threadPool.empty())
	{
		return nullptr;
	}

	auto iter = _threadPool.find(id);
	if (iter == _threadPool.end())
	{
		return nullptr;
	}

	auto thread = iter->second;

	return thread;
}