#include "pch.h"


#include "PacketAnalyzer.h"

PacketAnalyzer::PacketAnalyzer()
{}

PacketAnalyzer::~PacketAnalyzer()
{}

PacketAnalyzer* PacketAnalyzer::GetInstance()
{
	static PacketAnalyzer packetManager;

	return &packetManager;
}

//-------------------------------------------------------------------
// ��Ŷ �м�
//-------------------------------------------------------------------
Packet* PacketAnalyzer::Analyzer(const char* rowPacket, size_t size)
{
	size_t offset = 0;
	PacketType type[1] = { (PacketType)0, };
	
	memcpy_s(type, sizeof(type), (void*)rowPacket, sizeof(type));
	offset += sizeof(type);

	Packet* packet = PacketFactory::GetInstance()->GetPacket((BYTE)type[0]);
	if (packet)
	{
		// 8 < 24
		if (offset < size)
		{
			// ���⼭�� packetType �� ������ �����͸� stream�� �ѱ���
			Stream stream((UCHAR*)rowPacket + offset, size - offset);
			packet->Decoding(stream);
		}
	}

	return packet;
}