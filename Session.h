#pragma once

struct SOCKET_DATA
{
	SOCKET _socket;
	SOCKADDR_IN _addrInfo;
};

enum class SESSION_TYPE
{
	SESSION_TYPE_TERMINAL,
	SESSION_TYPE_CLIENT,
};

class Package;

class Session
{
public:
	Session();
	virtual ~Session();

	virtual bool OnAccept(const SOCKET& clientSocket, const SOCKADDR_IN& addrInfo);
	
	virtual void OnSend(size_t transferSize) = 0;
	virtual void SendPacket(Packet* packet) = 0;

	virtual Package* OnRecv(size_t transferSize) = 0;
	virtual void RecvStandBy() {};

	virtual void OnClose(bool force = false);

	//void UpdateHeartBeat();

	// getFunc
	SOCKET& Socket();
	std::wstring ClientAddress();
	UINT64 id();
	char type();
	//time_t heartBeat();

	// setFunc
	void setId(UINT64 id);
	void setType(char type);

protected:
	SOCKET_DATA _socketData;
	UINT64 _id;
	char _type;
	time_t _lastHeartBeat;

	bool SetSocketOpt();	
};