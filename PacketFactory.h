#pragma once

//--------------------------------------------------------
// ��Ŷ ���� / Singleton Class
//--------------------------------------------------------
class PacketFactory
{
public:
	static PacketFactory* GetInstance();

	Packet* GetPacket(BYTE packetType);

private:
	PacketFactory();
	~PacketFactory();
};