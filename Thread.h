#pragma once

// 쓰레드 생성 매크로
#define MAKE_THREAD(className, process)  (new Thread(new std::thread(&className##::##process, this), L#className))

// 쓰레드 락 생성 매크로
#define MAKE_THREAD_LOCK(pLock, className, process)  (new Thread(pLock, new std::thread(&className##::##process, this), L#className))

#define GET_CURRENT_THREAD_ID  std::this_thread::get_id

class Lock;

class Thread
{
public:
	Thread(std::thread* thread, std::wstring name); // 쓰레드 생성자
	Thread(Lock* pLock, std::thread* thread, std::wstring name); // 락 생성자
	~Thread();

	// setFunc
	void SetLock(Lock* lock);

	// getFunc
	std::thread::id id();
	std::wstring& name();
	Lock* lock();

private:
	std::thread::id _id;
	std::wstring _name;
	std::thread* _thread;
	Lock* _lock; // 지금 걸린 락
};

//--------------------------------------------------------------------
// Thread를 관리 (Singleton Class)
//--------------------------------------------------------------------
class ThreadManager
{
public:
	static ThreadManager* GetInstance();

	HRESULT Initialize();

	void Put(Thread* thread);
	void Remove(std::thread::id id);
	void Release();

	Thread* at(std::thread::id id);

private:
	ThreadManager();
	~ThreadManager();

	//-------------------------------------------------------------------
	// hash_map / unordered_map 에서 get를 할때,
	// 라이브러리에서 버켓 index 에러가 난다
	// 그런 이유로 검증된 map으로 컨테이너 교체를 하자
	// (성능상으론 map보다 hash_map이 더 좋다)
	//-------------------------------------------------------------------
	std::map<std::thread::id, Thread*> _threadPool;
};