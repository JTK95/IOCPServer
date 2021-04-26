//#pragma once
//
//#define INFO_COUNT 1000
//
////------------------------------------------------------------------------
//// new �����ڿ����ε����� ���� �޸� ���� ����
////------------------------------------------------------------------------
//class MemoryLeak
//{
//public:
//	MemoryLeak();
//	~MemoryLeak();
//
//	void* MemoryAlloc(size_t size, const char* file, int line, bool arrayCheck);
//	void MemoryDelete(void* p, bool arrayCheck);
//
//private:
//	// ���� �̸�
//	char _fileName[_MAX_PATH];
//
//	// ���� �޸� ����
//	struct MemoryInfo
//	{
//		char s_fileName[_MAX_PATH];
//		void* _p;
//		int _size;
//		int _line;
//		bool _arrayCheck;
//	};
//
//	// ���� �޸� ���� �迭
//	MemoryInfo* _memoryInfo[INFO_COUNT] = { nullptr };
//};
//
//void* operator new(size_t size, const char* file, int line);
//
//void* operator new[](size_t size, const char* file, int line);
//
//void operator delete(void* p);
//
//void operator delete[](void* p);
//
//#define new new(__FILE__, __LINE__)