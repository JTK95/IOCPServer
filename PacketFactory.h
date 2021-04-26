#pragma once

//--------------------------------------------------------
// 패킷 생성 / Singleton Class
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