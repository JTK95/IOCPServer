#pragma once

#define NAME_SIZE 20

class PacketProcess : public ContentsProcess
{
public:
	PacketProcess();
	~PacketProcess();

	void RegistSubPacketFunc();
	
	// Login 패킷 기능
	static void C_REQ_REGISTER(Session* session, Packet* rowPacket);
	static void C_REQ_LOGIN(Session* session, Packet* rowPacket);

	// Lobby 패킷 기능
	static void C_REQ_CONNECT_ROOM(Session* session, Packet* rowPacket);
	static void C_NOTIFY_CONNECT_ROOM_CLIENT(Session* session, Packet* rowPacket);

	// InGame 패킷 기능
	static void C_NOTIFY_LOGIN_CLIENT(Session* session, Packet* rowPacket);
	static void C_REQ_PLAYER_MOVE(Session* session, Packet* rowPacket);
	static void C_REQ_BULLET_SHOOT(Session* session, Packet* rowPacket);
	static void C_REQ_COLLISION_CHECK(Session* session, Packet* rowPacket);

	// Chatting 패킷 기능
	static void C_REQ_CHAT_INPUT(Session* session, Packet* rowPacket);

	static void C_REQ_EXIT_ROOM(Session* session, Packet* rowPacket);

private:
};

