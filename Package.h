#pragma once

class Session;

//----------------------------------------------------------
// ��Ŷ ���� �޸� ����
//----------------------------------------------------------
class Package
{
public:
	Package(Session* session, Packet* packet);
	~Package();

	Session* _session;
	Packet* _packet;
};