#pragma once

//-----------------------------------------------------------
// Singleton Class
//-----------------------------------------------------------
class WinSock
{
public:
	static WinSock* GetInstance();

	HRESULT Initialize();
	void release();

private:
	WinSock();
	~WinSock();

	WSADATA _wsa;
};