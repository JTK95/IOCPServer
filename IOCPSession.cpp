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
// IoData ����� 0���� �ʱ�ȭ
//------------------------------------------------------------------------
void IoData::Clear()
{
	_buffer.fill(0);
	_totalBytes = 0;
	_currentBytes = 0;
}

//------------------------------------------------------------------------
// IO�� �� �ʿ����� üũ
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
// _totalBytes�� �����ϰ� ���� �Ǿ����� �� ���� �� ��Ŷ ũ�� ��ȯ
//------------------------------------------------------------------------
int IoData::setupTotalBytes()
{
	int offset = 0;
	int packetLen[1] = { 0, };

	if (_totalBytes == 0)
	{
		//---------------------------------------------------------------------------
		// ���⼭ _buffer.data()�� �ش� �ε����� �������� ���� ��ȯ�Ǵ°� �ƴ϶�,
		// �迭�� ù��° �ּҸ� ��ȯ�Ѵ�
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
// ������� �� ����, ����, ���۸� ������
//------------------------------------------------------------------------
WSABUF IoData::wsabuf()
{
	WSABUF wsaBuf;
	wsaBuf.buf = _buffer.data() + _currentBytes;
	wsaBuf.len = (ULONG)(_totalBytes - _currentBytes);

	return wsaBuf;
}

//------------------------------------------------------------------------
// _buffer�� stream�� ũ�⸦ �����, ���۸� �ڿ� �߰����ִ� ��Ŷ ����,
// _totalBytes ����
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

	// _buffer�� ù��° �ּ� ��ȯ
	char* buf = _buffer.data();

	// ��� ũ�� + ���� ������ ũ��
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
	// IO ���� üũ
	if (ret == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
	{
		printf_s("IOCPSession::CheckErrorIO() socket error : %d\n", WSAGetLastError());
		//SLog(L"! socket error : %d", WSAGetLastError());
	}
}

//---------------------------------------------------------------------
// ���� recv ���ְ� ���� üũ
//---------------------------------------------------------------------
void IOCPSession::Recv(WSABUF wsaBuf)
{
	DWORD flags = 0;
	DWORD recvBytes = 0;
	DWORD errorCode = WSARecv
	(
		_socketData._socket,    // ���� ����
		&wsaBuf,                // wsabuf ����ü �迭�� ����Ű�� ������
		1,                      // wsabuf�� ����ü ����
		&recvBytes,             // ������ �Է��� �Ϸ�Ǹ�, ���� �������� ����Ʈ ũ�⸦ ����
		&flags,                 // �Լ��� ȣ�� ����� ����
		_ioData[(int)(IO_OPERATION::IO_READ)].overlapped(), // ������ ����
		NULL
	);
	
	this->CheckErrorIO(errorCode);
}

//---------------------------------------------------------------------
// ����� üũ, wsabuf recv, totalbytes ����
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
// ������ �м�, �м��� ��� ��Ŷ�� ���� Package�� ����� �������� ����
//---------------------------------------------------------------------
Package* IOCPSession::OnRecv(size_t transferSize)
{
	int offset = 0;
	size_t packetLen = 0;
	size_t* packetSize = nullptr;
	size_t checkDataBuffer = 0;

	// offset�� totalbyte ����
	offset += _ioData[(int)(IO_OPERATION::IO_READ)].setupTotalBytes();

	// _currentByte ����
	if (this->IsRecving(transferSize))
	{
		return nullptr;
	}

	// buf�� ���� ��Ŷ�� main���۷� ����
	memcpy_s(&_mainBuffer + _mainBufferOffset, sizeof(_mainBuffer) - _mainBufferOffset,
		_ioData[(int)(IO_OPERATION::IO_READ)].GetBuffer(), *_ioData[(int)(IO_OPERATION::IO_READ)].GetCurrentBytes());

	// ������ ��ŭ offset�� �̵�
	_mainBufferOffset += *_ioData[(int)(IO_OPERATION::IO_READ)].GetCurrentBytes();
	_ioData[(int)(IO_OPERATION::IO_READ)].SetTotalBytes(_mainBufferOffset);

	while (true)
	{
		// packetLen�� ���� _mainBuffer�ȿ� ��Ŷ�� ó���ϴ� ����
		if (sizeof(packetLen) > _mainBufferOffset)
		{
			// _mainBuffer�ȿ� ���� ������ ���� �� break;
			//puts("IOCPSession::OnRecv() no Data in _mainBuffer");
			//SLog(L"no Data in _mainBuffer");
			break;
		}

		memcpy(&packetLen, &_mainBuffer, sizeof(int));
		
		// GetTotalBytes() �տ� ����� �о ������ totalbyte��� �Ҽ� ����
		if (packetLen > *_ioData[(int)(IO_OPERATION::IO_READ)].GetTotalBytes())
		{
			puts("IOCPSession::OnRecv() not enough Data to read packet in _mainBuffer");
			//SLog(L"not enough Data to read packet in _mainBuffer");
			break;
		}

		const size_t packetHeaderSize = sizeof(int);
		int packetDataSize = (int)(packetLen - packetHeaderSize);
		BYTE packetData[200] = { 0, };

		// packetData�� _mainBuffer�� �ִ� ��Ŷ�� �����͸�ŭ �߶� �־��ش�
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
			// packetStrorage�� �ִ� ����
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
// _ioData[IO_READ] ���� �ʱ�ȭ, ������ recv���·� �ٲ���
// wsabuf ����
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
// wsaBuf �۽�, ���� üũ
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
// ��Ŷ ������ // on�̸� ��Ŷ �۽�
//---------------------------------------------------------------------
void IOCPSession::OnSend(size_t transferSize)
{
	if (_ioData[(int)(IO_OPERATION::IO_WRITE)].needMoreIO(transferSize))
	{
		this->Send(_ioData[(int)(IO_OPERATION::IO_WRITE)].wsabuf());
	}
}

//---------------------------------------------------------------------
// packet�����͸� _ioData ���·� ���� �� send��
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