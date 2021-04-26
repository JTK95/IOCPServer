#include "pch.h"

#include "IOCPSession.h"

IoData::IoData()
	:_ioType(IO_OPERATION::IO_ERROR)
{
	memset(&_overlapped, 0, sizeof(_overlapped));

	this->Clear();
}

IoData::~IoData()
{}

//------------------------------------------------------------------------
// IoData 멤버를 0으로 초기화
//------------------------------------------------------------------------
void IoData::Clear()
{
	_buffer.fill(0);
	_totalBytes = 0;
	_currentBytes = 0;
}

//------------------------------------------------------------------------
// IO가 더 필요한지 체크
//------------------------------------------------------------------------
BOOL IoData::needMoreIO(size_t transferSize)
{
	_currentBytes += transferSize;
	if (_currentBytes < _totalBytes)
	{
		return TRUE;
	}

	return FALSE;
}

//------------------------------------------------------------------------
// _totalBytes를 설정하고 설정 되어있을 시 무시 후 패킷 크기 반환
//------------------------------------------------------------------------
int IoData::setupTotalBytes()
{
	int offset = 0;
	int packetLen[1] = { 0, };

	if (_totalBytes == 0)
	{
		//---------------------------------------------------------------------------
		// 여기서 _buffer.data()는 해당 인덱스의 데이터의 값이 반환되는게 아니라,
		// 배열의 첫번째 주소를 반환한다
		//---------------------------------------------------------------------------
		memcpy_s((void*)packetLen, sizeof(packetLen), (const void*)_buffer.data(), sizeof(packetLen));

		_totalBytes += (size_t)packetLen[0];
	}
	else
	{
		memcpy_s((void*)packetLen, sizeof(packetLen), (const void*)_buffer.data(), sizeof(packetLen));

		_totalBytes += (size_t)packetLen[0];

		return offset;
	}

	offset += sizeof(packetLen);

	return offset;
}

//------------------------------------------------------------------------
// 현재까지 온 버퍼, 길이, 버퍼를 리턴함
//------------------------------------------------------------------------
WSABUF IoData::wsabuf()
{
	WSABUF wsaBuf;
	wsaBuf.buf = _buffer.data() + _currentBytes;
	wsaBuf.len = (ULONG)(_totalBytes - _currentBytes);

	return wsaBuf;
}

//------------------------------------------------------------------------
// _buffer에 stream의 크기를 헤더로, 버퍼를 뒤에 추가해주는 패킷 만듬,
// _totalBytes 갱신
//------------------------------------------------------------------------
BOOL IoData::SetData(Stream& stream)
{
	this->Clear();

	if (_buffer.max_size() <= stream.size())
	{
		printf_s("IoData::SetData() packet over size [%llu]byte\n", stream.size());
		//SLog(L"! packet size too bif [%d]byte", stream.size());

		return FALSE;
	}

	const size_t packetHeaderSize = sizeof(int);
	int offset = 0;	

	// _buffer의 첫번째 주소 반환
	char* buf = _buffer.data();

	// 헤더 크기 + 실제 데이터 크기
	int packetLen[1] = { packetHeaderSize + stream.size() };
	
	// insert packet len
	memcpy_s(buf + offset, _buffer.max_size(), packetLen, packetHeaderSize);
	offset += packetHeaderSize;

	// insert packet data
	memcpy_s(buf + offset, _buffer.max_size(), stream.data(), stream.size());
	offset += stream.size();
	
	_totalBytes = offset;

	return TRUE;
}

size_t* IoData::GetCurrentBytes()
{
	return &_currentBytes;
}

size_t* IoData::GetTotalBytes()
{
	return &_totalBytes;
}

std::array<char, 1024 * 10>* IoData::GetBuffer()
{
	return &_buffer;
}

void IoData::SetTotalBytes(size_t setTotalBytes)
{
	_totalBytes = setTotalBytes;
}

size_t IoData::TotalBytes()
{
	return _totalBytes;
}

IO_OPERATION& IoData::type()
{
	return _ioType;
}

char* IoData::data()
{
	return _buffer.data();
}

LPWSAOVERLAPPED IoData::overlapped()
{
	return &_overlapped;
}

void IoData::SetType(IO_OPERATION type)
{
	_ioType = type;
}

//---------------------------------------------------------------------
// IOCPSession class
//---------------------------------------------------------------------
IOCPSession::IOCPSession()
	:Session()
{
	//this->Initialize();
	InitializeSRWLock(&_lock);

	//memset(&_mainBuffer, 0, sizeof(_mainBuffer));
	memset(&_socketData, 0, sizeof(SOCKET_DATA));
	_ioData[(int)(IO_OPERATION::IO_READ)].SetType(IO_OPERATION::IO_READ);
	_ioData[(int)(IO_OPERATION::IO_WRITE)].SetType(IO_OPERATION::IO_WRITE);

}

void IOCPSession::CheckErrorIO(DWORD ret)
{
	// IO 에러 체크
	if (ret == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
	{
		printf_s("IOCPSession::CheckErrorIO() socket error : %d\n", WSAGetLastError());
		//SLog(L"! socket error : %d", WSAGetLastError());
	}
}

//---------------------------------------------------------------------
// 소켓 recv 해주고 에러 체크
//---------------------------------------------------------------------
void IOCPSession::Recv(WSABUF wsaBuf)
{
	DWORD flags = 0;
	DWORD recvBytes = 0;
	DWORD errorCode = WSARecv
	(
		_socketData._socket,    // 연결 소켓
		&wsaBuf,                // wsabuf 구조체 배열을 가리키는 포인터
		1,                      // wsabuf의 구조체 개수
		&recvBytes,             // 데이터 입력이 완료되면, 읽은 데이터의 바이트 크기를 받음
		&flags,                 // 함수의 호출 방식을 지정
		_ioData[(int)(IO_OPERATION::IO_READ)].overlapped(), // 오버랩 설정
		NULL
	);
	
	this->CheckErrorIO(errorCode);
}

//---------------------------------------------------------------------
// 입출력 체크, wsabuf recv, totalbytes 설정
//---------------------------------------------------------------------
bool IOCPSession::IsRecving(size_t transferSize)
{
	if (_ioData[(int)(IO_OPERATION::IO_READ)].needMoreIO(transferSize))
	{
		this->Recv(_ioData[(int)(IO_OPERATION::IO_READ)].wsabuf());
		_ioData[(int)(IO_OPERATION::IO_READ)].setupTotalBytes();
		
		return true;
	}

	return false;
}

//---------------------------------------------------------------------
// 데이터 분석, 분석한 대로 패킷을 얻어와 Package를 만들어 내보내는 역할
//---------------------------------------------------------------------
Package* IOCPSession::OnRecv(size_t transferSize)
{
	int offset = 0;
	size_t packetLen = 0;
	size_t* packetSize = nullptr;
	size_t checkDataBuffer = 0;

	// offset에 totalbyte 설정
	offset += _ioData[(int)(IO_OPERATION::IO_READ)].setupTotalBytes();

	// _currentByte 설정
	if (this->IsRecving(transferSize))
	{
		return nullptr;
	}

	// buf로 받은 패킷을 main버퍼로 복사
	memcpy_s(&_mainBuffer + _mainBufferOffset, sizeof(_mainBuffer) - _mainBufferOffset,
		_ioData[(int)(IO_OPERATION::IO_READ)].GetBuffer(), *_ioData[(int)(IO_OPERATION::IO_READ)].GetCurrentBytes());

	// 복사한 만큼 offset을 이동
	_mainBufferOffset += *_ioData[(int)(IO_OPERATION::IO_READ)].GetCurrentBytes();
	_ioData[(int)(IO_OPERATION::IO_READ)].SetTotalBytes(_mainBufferOffset);

	while (true)
	{
		// packetLen을 보고 _mainBuffer안에 패킷을 처리하는 구문
		if (sizeof(packetLen) > _mainBufferOffset)
		{
			// _mainBuffer안에 읽을 내용이 없을 때 break;
			//puts("IOCPSession::OnRecv() no Data in _mainBuffer");
			//SLog(L"no Data in _mainBuffer");
			break;
		}

		memcpy(&packetLen, &_mainBuffer, sizeof(int));
		
		// GetTotalBytes() 앞에 헤더만 읽어서 진정한 totalbyte라고 할수 없음
		if (packetLen > *_ioData[(int)(IO_OPERATION::IO_READ)].GetTotalBytes())
		{
			puts("IOCPSession::OnRecv() not enough Data to read packet in _mainBuffer");
			//SLog(L"not enough Data to read packet in _mainBuffer");
			break;
		}

		const size_t packetHeaderSize = sizeof(int);
		int packetDataSize = (int)(packetLen - packetHeaderSize);
		BYTE packetData[200] = { 0, };

		// packetData에 _mainBuffer에 있는 패킷의 데이터만큼 잘라서 넣어준다
		memcpy(packetData, _mainBuffer + offset, packetDataSize);

		Packet* packet = PacketAnalyzer::GetInstance()->Analyzer((const char*)packetData, packetDataSize);
		if (packet == nullptr)
		{
			printf_s("IOCPSession::OnRecv() Invalid packet [%d]\n", packet->type());
			//SLog(L"! [%d],invalid packet", packet->type());
			this->OnClose(true);
			_ioData[(int)(IO_OPERATION::IO_READ)].SetTotalBytes(_mainBufferOffset);
			break;
		}

		//printf_s("packet type : %d\n", packet->type());
		//SLog(L"%d", packet->type());

		Package* package = new Package(this, packet);
		if (packet != NULL)
		{
			// packetStrorage에 넣는 구문
			this->PutPackage(package);
		}

		memcpy(_mainBuffer, _mainBuffer + packetLen, sizeof(_mainBuffer) - packetLen);
		_mainBufferOffset -= packetLen;
		_ioData[(int)(IO_OPERATION::IO_READ)].SetTotalBytes(_mainBufferOffset);
	}

	this->RecvStandBy();

	return NULL;
}

//---------------------------------------------------------------------
// _ioData[IO_READ] 버퍼 초기화, 소켓을 recv상태로 바꿔줌
// wsabuf 셋팅
//---------------------------------------------------------------------
void IOCPSession::RecvStandBy()
{
	_ioData[(int)(IO_OPERATION::IO_READ)].Clear();

	WSABUF wsaBuf;
	wsaBuf.buf = _ioData[(int)(IO_OPERATION::IO_READ)].data();
	wsaBuf.len = SOCKET_BUF_SIZE;

	this->Recv(wsaBuf);
}

//---------------------------------------------------------------------
// wsaBuf 송신, 에러 체크
//---------------------------------------------------------------------
void IOCPSession::Send(WSABUF wsaBuf)
{
	DWORD flags = 0;
	DWORD sendBytes;
	DWORD errorCode = WSASend
	(
		_socketData._socket,
		&wsaBuf,
		1,
		&sendBytes,
		flags,
		_ioData[(int)(IO_OPERATION::IO_WRITE)].overlapped(),
		NULL
	);
	
	this->CheckErrorIO(errorCode);
}

//---------------------------------------------------------------------
// 패킷 보내기 // on이면 패킷 송신
//---------------------------------------------------------------------
void IOCPSession::OnSend(size_t transferSize)
{
	if (_ioData[(int)(IO_OPERATION::IO_WRITE)].needMoreIO(transferSize))
	{
		this->Send(_ioData[(int)(IO_OPERATION::IO_WRITE)].wsabuf());
	}
}

//---------------------------------------------------------------------
// packet데이터를 _ioData 형태로 변형 후 send함
//---------------------------------------------------------------------
void IOCPSession::SendPacket(Packet* packet)
{
	Stream stream;
	packet->Encoding(stream);
	if (!_ioData[(int)(IO_OPERATION::IO_WRITE)].SetData(stream))
	{
		return;
	}

	WSABUF wsaBuf;
	wsaBuf.buf = _ioData[(int)(IO_OPERATION::IO_WRITE)].data();
	wsaBuf.len = stream.size() + 4;// sizeof(int)
	
	this->Send(wsaBuf);
}

bool IOCPSession::IsEmpty()
{
	bool temp;
	
	AcquireSRWLockShared(&_lock);
	temp = _packageStorage.empty();
	ReleaseSRWLockShared(&_lock);

	return temp;
}

void IOCPSession::PutPackage(Package* package)
{
	AcquireSRWLockExclusive(&_lock);
	_packageStorage.push_back(package);
	ReleaseSRWLockExclusive(&_lock);
}

Package* IOCPSession::PopPackage()
{
	Package* temp;

	AcquireSRWLockExclusive(&_lock);
	temp = _packageStorage.front();
	_packageStorage.pop_front();
	ReleaseSRWLockExclusive(&_lock);

	return temp;
}