#include "pch.h"

#include "WinSock.h"

WinSock::WinSock()
{}

WinSock::~WinSock()
{}

WinSock* WinSock::GetInstance()
{
	static WinSock winsockManager;

	return &winsockManager;
}

//-------------------------------------------------------------------------
// 윈속 초기화
//-------------------------------------------------------------------------
HRESULT WinSock::Initialize()
{
	//-------------------------------------------------
	// 윈속 버전 요청
	// 윈속 라이브러리(WS2_32.DLL)를 초기화
	//-------------------------------------------------
	if (WSAStartup(MAKEWORD(2, 2), &_wsa) != 0)
	{
		puts("WinSock::Initialize() WSAStartUp Fail");
		return E_FAIL;
	}

	puts("### WSA 2.2 Set complet... ###\n");

	return S_OK;
}

void WinSock::release()
{
	// 윈속 종료
	WSACleanup();
}