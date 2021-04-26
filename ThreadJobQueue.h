#pragma once

#include "pch.h"

template<class T>
class ThreadJobQueue
{
public:
	ThreadJobQueue(const WCHAR* name)
		:_lock(name), _writeQueue(&_queue[WRITE_QUEUE]), _readQueue(&_queue[READ_QUEUE])
	{}

	~ThreadJobQueue()
	{
		_readQueue->empty();
		_writeQueue->empty();
	}

	//----------------------------------------------------------
	// _writeQueue에 데이터를 push
	//----------------------------------------------------------
	inline void push(const T& t)
	{
		SAFE_LOCK(_lock);
		_writeQueue->push(t);
	}

	//----------------------------------------------------------
	// _readQueue의 첫번째 데이터를 pop
	//----------------------------------------------------------
	inline bool pop(_Out_ T& t)
	{
		SAFE_LOCK(_lock);

		size_t size = this->size();
		if (size == 0)
		{
			return false;
		}

		if (_readQueue->empty())
		{
			this->swap();
		}

		t = _readQueue->front();
		
		// front값을 빼줌
		_readQueue->pop();

		return true;
	}

	//----------------------------------------------------------
	// _readQueue와 _writeQueue를 스왑
	//----------------------------------------------------------
	inline void swap()
	{
		SAFE_LOCK(_lock);

		if (_writeQueue == &_queue[WRITE_QUEUE])
		{
			_writeQueue = &_queue[READ_QUEUE];
			_readQueue = &_queue[WRITE_QUEUE];
		}
		else
		{
			_writeQueue = &_queue[WRITE_QUEUE];
			_readQueue = &_queue[READ_QUEUE];
		}
	}

	inline bool IsEmpty() const
	{
		return _readQueue->empty();
	}

	//----------------------------------------------------------
	// writequeue + readqueue
	//----------------------------------------------------------
	inline size_t size()
	{
		SAFE_LOCK(_lock);

		size_t size = (size_t)(_queue[WRITE_QUEUE].size() + _queue[READ_QUEUE].size());

		return size;
	}

private:
	enum
	{
		WRITE_QUEUE,
		READ_QUEUE,
		MAX_QUEUE
	};

	std::queue<T> _queue[MAX_QUEUE];
	std::queue<T>* _writeQueue; // 여러 쓰레드 입력 받음
	std::queue<T>* _readQueue; // 한 쓰레드에서 출력 함

	Lock _lock;
};