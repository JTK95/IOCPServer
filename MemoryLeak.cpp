//#include "pch.h"
//
//#include "MemoryLeak.h"
//
//#undef new
//
////-----------------------------------------------------------
//// 생성자
////-----------------------------------------------------------
//MemoryLeak::MemoryLeak()
//{
//	// 파일 이름 설정
//	tm timeInfo;
//	time_t nowTime = time(NULL);
//	localtime_s(&timeInfo, &nowTime);
//
//	sprintf_s(_fileName, "Alloc_%d%d%d_%d%d%d.txt", timeInfo.tm_year + 1900, timeInfo.tm_mon + 1,
//		timeInfo.tm_mday, timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec);
//}
//
////-----------------------------------------------------------
//// 소멸자
////-----------------------------------------------------------
//MemoryLeak::~MemoryLeak()
//{
//	FILE* pFile;
//	fopen_s(&pFile, _fileName, "at");
//
//	for (int i = 0; i < INFO_COUNT; i++)
//	{
//		if (_memoryInfo[i] == nullptr)
//		{
//			continue;
//		}
//
//		char errorMsg[_MAX_PATH];
//		sprintf_s(errorMsg, "LEAK   [0x%p] [   %d] %s : %d \n", _memoryInfo[i]->_p,
//			_memoryInfo[i]->_size, _memoryInfo[i]->s_fileName, _memoryInfo[i]->_line);
//		fwrite(errorMsg, strlen(errorMsg), 1, pFile);
//	}
//
//	fclose(pFile);
//}
//
////-----------------------------------------------------------
//// 메모리 할당 정보 등록
////-----------------------------------------------------------
//void* MemoryLeak::MemoryAlloc(size_t size, const char* file, int line, bool arrayCheck)
//{
//	void* p = malloc(size);
//
//	for (int i = 0; i < INFO_COUNT; i++)
//	{
//		if (_memoryInfo[i] != nullptr)
//		{
//			continue;
//		}
//
//		_memoryInfo[i] = (MemoryInfo*)malloc(sizeof(MemoryInfo));
//		StringCchCopyA(_memoryInfo[i]->s_fileName, _MAX_PATH, file);
//		_memoryInfo[i]->_line = line;
//		_memoryInfo[i]->_p = p;
//		_memoryInfo[i]->_size = size;
//		_memoryInfo[i]->_arrayCheck = arrayCheck;
//		break;
//	}
//
//	return p;
//}
//
////-----------------------------------------------------------
//// 메모리 할당 정보 해제
////-----------------------------------------------------------
//void MemoryLeak::MemoryDelete(void* p, bool arrayCheck)
//{
//	for (int i = 0; i < INFO_COUNT; i++)
//	{
//		if (_memoryInfo[i] == nullptr)
//		{
//			continue;
//		}
//
//		if (_memoryInfo[i]->_p != p)
//		{
//			continue;
//		}
//
//		// 에러 출력 후 반환
//		if (_memoryInfo[i]->_arrayCheck == arrayCheck)
//		{
//			char error[128];
//			sprintf_s(error, "ARRAY   [0x%p] [   %d] %s : %d \n", _memoryInfo[i]->_p,
//				_memoryInfo[i]->_size, _memoryInfo[i]->s_fileName, _memoryInfo[i]->_line);
//
//			FILE* fp;
//			fopen_s(&fp, _fileName, "at");
//			fwrite(error, strlen(error), 1, fp);
//			fclose(fp);
//
//			return;
//		}
//
//		free(_memoryInfo[i]);
//		free(p);
//		_memoryInfo[i] = nullptr;
//
//		return;
//	}
//
//	// NOALLOC 출력 로직부
//	char errorMsg[128];
//	sprintf_s(errorMsg, "NOALLOC [0x%p]\n", p);
//
//	FILE* fp;
//	fopen_s(&fp, _fileName, "at");
//	fwrite(errorMsg, strlen(errorMsg), 1, fp);
//	fclose(fp);
//}
//
//MemoryLeak leakObject;
//
//void* operator new(size_t size, const char* file, int line)
//{
//	return leakObject.MemoryAlloc(size, file, line, false);
//}
//
//void* operator new[](size_t size, const char* file, int line)
//{
//	return leakObject.MemoryAlloc(size, file, line, true);
//}
//
//void operator delete(void* p)
//{
//	leakObject.MemoryDelete(p, true);
//}
//
//void operator delete[](void* p)
//{
//	leakObject.MemoryDelete(p, false);
//}