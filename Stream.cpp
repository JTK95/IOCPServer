#include "pch.h"


#include "Stream.h"

Stream::Stream()
{
	this->Initialize();
}

Stream::Stream(UCHAR* stream, size_t size)
{
	this->Initialize();
	this->Set(stream, size);
}

Stream::~Stream()
{}

void Stream::Initialize()
{
	memset(&_stream, 0, sizeof(_stream));
	_readPt = 0;
	_offset = 0;
}

UCHAR* Stream::data()
{
	return _stream.data();
}

size_t Stream::size()
{
	return _offset;
}

void Stream::operator=(Stream& stream)
{
	this->Set(stream.data(), stream.size());
}

void Stream::Set(UCHAR* data, size_t size)
{
	_offset = size;
	memcpy_s(_stream.data(), _stream.size(), data, size);
}

//------------------------------------------------------------------------
// write
//------------------------------------------------------------------------
bool Stream::CheckWriteBound(size_t len)
{
	if (_offset + len > sizeof(_stream))
	{
		puts("CStream::CheckWriteBound() Socket Over Stream");
		//SLog(L"! socket stream over");
		ASSERT(FALSE);

		return false;
	}

	return true;
}

#define STREAM_WRITE(value)                                    \
    INT32 size = sizeof(value);                                \
    if(this->CheckWriteBound(size) == false)                   \
      {                                                        \
         return;                                               \
      }                                                        \
    memcpy_s((void*)(_stream.data() + _offset), _stream.size() - _offset, (const void*)&value, size);\
    _offset += size;                              

template<class T>
void Stream::operator<<(const T& value)
{
	STREAM_WRITE(value)
}

void Stream::operator<<(const bool& value)
{
	STREAM_WRITE(value)
}

void Stream::operator<<(const char& value)
{
	STREAM_WRITE(value)
}

void Stream::operator<<(const BYTE& value)
{
	STREAM_WRITE(value)
}

void Stream::operator<<(const WCHAR& value)
{
	STREAM_WRITE(value)
}

void Stream::operator<<(const WORD& value)
{
	STREAM_WRITE(value)
}

void Stream::operator<<(const float& value)
{
	STREAM_WRITE(value)
}

void Stream::operator<<(const int& value)
{
	STREAM_WRITE(value)
}

void Stream::operator<<(const __int64& value)
{
	STREAM_WRITE(value)
}

void Stream::operator<<(const std::string value)
{
	*this << (INT32)value.length();
	for (auto i : value)
	{
		*this << i;
	}
}

void Stream::operator<<(const std::wstring value)
{
	*this << (INT32)value.length();
	for (auto i : value)
	{
		*this << i;
	}
}

void Stream::operator<<(const std::vector<std::wstring>& value)
{
	*this << value.size();
	for (auto i : value)
	{
		*this << i;
	}
}

//------------------------------------------------------------------------
// read
//------------------------------------------------------------------------
bool Stream::CheckReadBound(size_t len)
{
	if (_readPt + len > _offset)
	{
		printf_s("CStream::CheckReadBound() readOffset : %d, size : %d, totalOffset : %d\n", _readPt, len, _offset);
		puts("CStream::CheckReadBound() socket stream has not more memory");
		/*SLog(L"! readOffset : %d. size : %d, totalOffset : %d", _readPt, len, _offset);
		SLog(L"! socket stream has not more memory");*/
		ASSERT(FALSE);

		return false;
	}

	return true;
}

void Stream::Read(void* retVal, size_t len)
{
	memcpy_s(retVal, len, _stream.data() + _readPt, len);
	_readPt += len;
}

#define STREAM_READ(type, retVal)                \
    size_t size = sizeof(type);                  \
    if(this->CheckReadBound(size) == false)      \
      {                                          \
         return;                                 \
      }                                          \
    this->Read((void*)retVal, size);

template<class T>
void Stream::operator>>(T* retVal)
{
	STREAM_READ(T, retVal);
}

void Stream::operator>>(bool* retVal)
{
	STREAM_READ(bool, retVal);
}

void Stream::operator>>(char* retVal)
{
	STREAM_READ(char, retVal);
}

void Stream::operator>>(BYTE* retVal)
{
	STREAM_READ(BYTE, retVal);
}

void Stream::operator>>(WCHAR* retVal)
{
	STREAM_READ(char, retVal);
}

void Stream::operator>>(WORD* retVal)
{
	STREAM_READ(WORD, retVal);
}

void Stream::operator>>(float* retVal)
{
	STREAM_READ(float, retVal);
}

void Stream::operator>>(int* retVal)
{
	STREAM_READ(int, retVal);
}

void Stream::operator>>(__int64* retVal)
{
	STREAM_READ(__int64, retVal);
}

void Stream::operator>>(std::string* retVal)
{
	INT32 size;
	*this >> &size;
	
	if (this->CheckReadBound(size) == false)
	{
		return;
	}

	char* buffer = new char[size + 1];
	this->Read((void*)buffer, size * sizeof(char));
	buffer[size] = '\0';

	retVal->clear();
	*retVal = buffer;

	delete buffer;
}

void Stream::operator>>(std::wstring* retVal)
{
	INT32 size;
	*this >> &size;

	if (this->CheckReadBound(size) == false)
	{
		return;
	}

	WCHAR* buffer = new WCHAR[size + 1];
	this->Read((void*)buffer, size * sizeof(WCHAR));
	buffer[size] = '\0';

	retVal->clear();
	*retVal = buffer;

	delete buffer;
}

void Stream::operator>>(std::vector<std::wstring>* retVal)
{
	size_t size;
	*this >> &size;

	for (size_t i = 0; i < size; ++i)
	{
		std::wstring tmp;
		*this >> &tmp;
		retVal->push_back(tmp);
	}
}