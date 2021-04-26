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
// 소켓 옵션(좀비 소켓 체크) 설정
//----------------------------------------------------------------------------
bool Session::SetSocketOpt()
{
	// 좀비 소켓을 체크하기 위함
	tcp_keepalive keepAliveSet = { 0 }; // 인버퍼
	tcp_keepalive returned = { 0 }; // 아웃버퍼
	keepAliveSet.onoff = 1; // KEEPALIVE ON
	keepAliveSet.keepalivetime = 3000; // Keep Alive in 3 Sec.
	keepAliveSet.keepaliveinterval = 3000; // 응답이 없으면 재전송 (10회 반복)

	DWORD dwBytes;

	// 소켓 모드 제어
	if (WSAIoctl
	(
		_socketData._socket,     // 소켓
		SIO_KEEPALIVE_VALS,      // 소켓이 살아있나 체크하는 옵션
		&keepAliveSet,           // 소켓 체크 셋팅 (인버퍼)
		sizeof(keepAliveSet),    // 인버퍼 크기
		&returned,               // 아웃버퍼
		sizeof(returned),        // 아웃 버퍼 크기
		&dwBytes,                // IOCP에서 리턴 된 바이트 크기
		NULL,                    // 비동기 소켓에서는 무시
		NULL
	) != 0)
	{
		return false;
	}

	return true;
}

//---------------------------------------------------------------------------
// accept한 소켓 데이터 처리, 소켓 옵션 처리
//---------------------------------------------------------------------------
bool Session::OnAccept(const SOCKET& clientSocket, const SOCKADDR_IN& addrInfo)
{
	_socketData._socket = clientSocket;
	int addrLen;

	// 연결된 상대측 소켓 주소 정보를 가져온다
	getpeername
	(
		_socketData._socket,                 // 연결한 소켓
		(sockaddr*)&_socketData._addrInfo,   // 연결한 소켓 주소정보가 여기로 넘어온다
		&addrLen                             // 주소정보 크기
	);

	//_socketData._addrInfo = addrInfo;
	
	if (!this->SetSocketOpt())
	{
		return false;
	}

	return true;
}

//------------------------------------------------------------------------
// 소켓을 닫아주는 함수
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