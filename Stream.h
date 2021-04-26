#pragma once

#define SOCKET_BUF_SIZE 1024*10

//-----------------------------------------------------
// Á÷·ÄÈ­ class
//-----------------------------------------------------
class Stream
{
public:
	Stream();
	Stream(UCHAR* stream, size_t size);
	~Stream();

	void Initialize();

	UCHAR* data();
	size_t size();

	void operator=(Stream& stream);
	void Set(UCHAR* data, size_t size);
	
	// write
	bool CheckWriteBound(size_t len);

	template<class T>
	void operator<<(const T& value);
	void operator<<(const bool& value);
	void operator<<(const char& value);
	void operator<<(const BYTE& value);
	void operator<<(const WCHAR& value);
	void operator<<(const WORD& value);
	void operator<<(const float& value);
	void operator<<(const int& value);
	void operator<<(const __int64& value);
	void operator<<(const std::string value);
	void operator<<(const std::wstring value);

	void operator << (const std::vector<std::wstring>& value);

	// read
	bool CheckReadBound(size_t len);
	void Read(void* retVal, size_t len);

	template<class T>
	void operator>>(T* retVal);
	void operator>>(bool* retVal);
	void operator>>(char* retVal);
	void operator>>(BYTE* retVal);
	void operator>>(WCHAR* value);
	void operator>>(WORD* value);
	void operator>>(float* retVal);
	void operator>>(int* retVal);
	void operator>>(__int64* retVal);
	void operator>>(std::string* retVal);
	void operator>>(std::wstring* retVal);

	void operator >> (std::vector<std::wstring>* retVal);

private:
	BYTE _buffer[SOCKET_BUF_SIZE];

	size_t _offset;
	size_t _readPt;
	std::array<byte, SOCKET_BUF_SIZE> _stream;
};

