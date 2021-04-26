#include "pch.h"

#include "Session.h"

Session::Session()
	:_type((int)SESSION_TYPE::SESSION_TYPE_CLIENT)
{}

Session::~Session()
{
	this->OnClose();
}

//----------------------------------------------------------------------------
// ���� �ɼ�(���� ���� üũ) ����
//----------------------------------------------------------------------------
bool Session::SetSocketOpt()
{
	// ���� ������ üũ�ϱ� ����
	tcp_keepalive keepAliveSet = { 0 }; // �ι���
	tcp_keepalive returned = { 0 }; // �ƿ�����
	keepAliveSet.onoff = 1; // KEEPALIVE ON
	keepAliveSet.keepalivetime = 3000; // Keep Alive in 3 Sec.
	keepAliveSet.keepaliveinterval = 3000; // ������ ������ ������ (10ȸ �ݺ�)

	DWORD dwBytes;

	// ���� ��� ����
	if (WSAIoctl
	(
		_socketData._socket,     // ����
		SIO_KEEPALIVE_VALS,      // ������ ����ֳ� üũ�ϴ� �ɼ�
		&keepAliveSet,           // ���� üũ ���� (�ι���)
		sizeof(keepAliveSet),    // �ι��� ũ��
		&returned,               // �ƿ�����
		sizeof(returned),        // �ƿ� ���� ũ��
		&dwBytes,                // IOCP���� ���� �� ����Ʈ ũ��
		NULL,                    // �񵿱� ���Ͽ����� ����
		NULL
	) != 0)
	{
		return false;
	}

	return true;
}

//---------------------------------------------------------------------------
// accept�� ���� ������ ó��, ���� �ɼ� ó��
//---------------------------------------------------------------------------
bool Session::OnAccept(const SOCKET& clientSocket, const SOCKADDR_IN& addrInfo)
{
	_socketData._socket = clientSocket;
	int addrLen;

	// ����� ����� ���� �ּ� ������ �����´�
	getpeername
	(
		_socketData._socket,                 // ������ ����
		(sockaddr*)&_socketData._addrInfo,   // ������ ���� �ּ������� ����� �Ѿ�´�
		&addrLen                             // �ּ����� ũ��
	);

	//_socketData._addrInfo = addrInfo;
	
	if (!this->SetSocketOpt())
	{
		return false;
	}

	return true;
}

//------------------------------------------------------------------------
// ������ �ݾ��ִ� �Լ�
//------------------------------------------------------------------------
void Session::OnClose(bool force)
{
	if (force)
	{
		SessionManager::GetInstance()->forceCloseSession(this);
	}
	else
	{
		SessionManager::GetInstance()->CloseSession(this);
	}
}

SOCKET& Session::Socket()
{
	return _socketData._socket;
}

std::wstring Session::ClientAddress()
{
	std::array<char, 64> ip;
	
	inet_ntop
	(
		AF_INET,
		&(_socketData._addrInfo.sin_addr),
		ip.data(),
		ip.size()
	);

	std::array<WCHAR, 64> wip;
	MultiByteToWideChar
	(
		CP_ACP,
		0,
		ip.data(),
		-1,
		wip.data(),
		wip.max_size()
	);

	std::wstring stringData = wip.data();

	return stringData;
}

UINT64 Session::id()
{
	return _id;
}

char Session::type()
{
	return _type;
}

//time_t Session::heartBeat()
//{
//	return _lastHeartBeat;
//}

void Session::setId(UINT64 id)
{
	_id = id;
}

void Session::setType(char type)
{
	_type = type;
}