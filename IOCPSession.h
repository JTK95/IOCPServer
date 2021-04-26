#pragma once

enum class IO_OPERATION
{
	IO_READ,
	IO_WRITE,
	IO_ERROR
};

#define IO_DATA_MAX 2

class IoData
{
public:
	IoData();
	~IoData();

	void Clear();

	BOOL needMoreIO(size_t transferSize);
	int setupTotalBytes();

	WSABUF wsabuf();
	BOOL SetData(Stream& stream);
	
	size_t* GetCurrentBytes();
	size_t* GetTotalBytes();
	std::array<char, 1024 * 10>* GetBuffer();
	
	void SetTotalBytes(size_t setTotalBytes);

	// getFunc
	size_t TotalBytes();
	IO_OPERATION& type();
	char* data();
	LPWSAOVERLAPPED overlapped();

	// setFunc
	void SetType(IO_OPERATION type);

private:
	OVERLAPPED _overlapped;
	IO_OPERATION _ioType;
	size_t _totalBytes;
	size_t _currentBytes;
	std::array<char, SOCKET_BUF_SIZE> _buffer;
};

//--------------------------------------------------------
// IOCPSession
//--------------------------------------------------------
class IOCPSession : public Session
{
public:
	IOCPSession();
	virtual ~IOCPSession(){}

	virtual void OnSend(size_t transferSize);
	virtual void SendPacket(Packet* packet);

	virtual Package* OnRecv(size_t transferSize);
	virtual void RecvStandBy();

	bool IsEmpty();
	void PutPackage(Package* package);
	Package* PopPackage();

public:
	std::array<IoData, IO_DATA_MAX> _ioData;
	BYTE _mainBuffer[1024 * 1000];
	int _mainBufferOffset;
	std::list<Package*> _packageStorage;

private:
	void CheckErrorIO(DWORD ret);

	void Recv(WSABUF wsaBuf);
	bool IsRecving(size_t transferSize);

	void Send(WSABUF wsaBuf);
	
	SRWLOCK _lock;
};