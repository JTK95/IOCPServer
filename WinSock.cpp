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
// ���� �ʱ�ȭ
//-------------------------------------------------------------------------
HRESULT WinSock::Initialize()
{
	//-------------------------------------------------
	// ���� ���� ��û
	// ���� ���̺귯��(WS2_32.DLL)�� �ʱ�ȭ
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
	// ���� ����
	WSACleanup();
}