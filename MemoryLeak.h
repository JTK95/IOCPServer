//#pragma once
//
//#define INFO_COUNT 1000
//
////------------------------------------------------------------------------
//// new 연산자오버로딩으로 인한 메모리 누수 추적
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
//	// 파일 이름
//	char _fileName[_MAX_PATH];
//
//	// 동적 메모리 정보
//	struct MemoryInfo
//	{
//		char s_fileName[_MAX_PATH];
//		void* _p;
//		int _size;
//		int _line;
//		bool _arrayCheck;
//	};
//
//	// 동적 메모리 정보 배열
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