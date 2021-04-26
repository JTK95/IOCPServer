#pragma once

//-----------------------------------------------------------
// 패킷 분석기 Singleton Class
//-----------------------------------------------------------
class PacketAnalyzer
{
public:
	static PacketAnalyzer* GetInstance();

	Packet* Analyzer(const char* rowPacket, size_t size);

private:
	PacketAnalyzer();
	~PacketAnalyzer();
};