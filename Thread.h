#pragma once

// ������ ���� ��ũ��
#define MAKE_THREAD(className, process)  (new Thread(new std::thread(&className##::##process, this), L#className))

// ������ �� ���� ��ũ��
#define MAKE_THREAD_LOCK(pLock, className, process)  (new Thread(pLock, new std::thread(&className##::##process, this), L#className))

#define GET_CURRENT_THREAD_ID  std::this_thread::get_id

class Lock;

class Thread
{
public:
	Thread(std::thread* thread, std::wstring name); // ������ ������
	Thread(Lock* pLock, std::thread* thread, std::wstring name); // �� ������
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
	Lock* _lock; // ���� �ɸ� ��
};

//--------------------------------------------------------------------
// Thread�� ���� (Singleton Class)
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
	// hash_map / unordered_map ���� get�� �Ҷ�,
	// ���̺귯������ ���� index ������ ����
	// �׷� ������ ������ map���� �����̳� ��ü�� ����
	// (���ɻ����� map���� hash_map�� �� ����)
	//-------------------------------------------------------------------
	std::map<std::thread::id, Thread*> _threadPool;
};