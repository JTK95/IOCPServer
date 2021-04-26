#pragma once

class Session;

//----------------------------------------------------------
// 패킷 동적 메모리 해제
//----------------------------------------------------------
class Package
{
public:
	Package(Session* session, Packet* packet);
	~Package();

	Session* _session;
	Packet* _packet;
};