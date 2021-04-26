#include "pch.h"

#include "PacketFactory.h"

PacketFactory::PacketFactory()
{}

PacketFactory::~PacketFactory()
{}

PacketFactory* PacketFactory::GetInstance()
{
	static PacketFactory factoryManager;

	return &factoryManager;
}

Packet* PacketFactory::GetPacket(BYTE packetType)
{
	// enum과 달리 enum클래스는 묵시적 형 변환이 안 되기에 타입캐스팅을 해줘야 한다
	switch (packetType)
	{
	case (BYTE)PacketType::E_C_REQ_EXIT:
		return new PK_C_REQ_EXIT;
	case (BYTE)PacketType::E_S_ANS_EXIT:
		return new PK_S_ANS_EXIT;
	case (BYTE)PacketType::E_C_REQ_REGISTER:
		return new PK_C_REQ_REGISTER;
	case (BYTE)PacketType::E_S_ANS_REGISTER_FAIL:
		return new PK_S_ANS_REGISTER_FAIL;
	case (BYTE)PacketType::E_S_ANS_REGISTER_SUCCESS:
		return new PK_S_ANS_REGISTER_SUCCESS;
	case (BYTE)PacketType::E_C_REQ_LOGIN:
		return new PK_C_REQ_LOGIN;
	case (BYTE)PacketType::E_S_ANS_LOGIN_FAIL:
		return new PK_S_ANS_LOGIN_FAIL;
	case (BYTE)PacketType::E_S_ANS_LOGIN_SUCCESS:
		return new PK_S_ANS_LOGIN_SUCCESS;
	case (BYTE)PacketType::E_C_NOTIFY_LOGIN_CLIENT:
		return new PK_C_NOTIFY_LOGIN_CLIENT;
	case (BYTE)PacketType::E_S_NOTIFY_LOGIN_CLIENT:
		return new PK_S_NOTIFY_LOGIN_CLIENT;
	case (BYTE)PacketType::E_C_REQ_CONNECT_ROOM:
		return new PK_C_REQ_CONNECT_ROOM;
	case (BYTE)PacketType::E_S_ANS_CONNECT_ROOM:
		return new PK_S_ANS_CONNECT_ROOM;
	case (BYTE)PacketType::E_S_ANS_CONNECT_FAIL_ROOM:
		return new PK_S_ANS_CONNECT_FAIL_ROOM;
	case (BYTE)PacketType::E_C_NOTIFY_CONNECT_ROOM_CLIENT:
		return new PK_C_NOTIFY_CONNECT_ROOM_CLIENT;
	case (BYTE)PacketType::E_S_NOTIFY_CONNECT_ROOM_CLIENT:
		return new PK_S_NOTIFY_CONNECT_ROOM_CLIENT;
	case (BYTE)PacketType::E_C_REQ_PLAYER_MOVE:
		return new PK_C_REQ_PLAYER_MOVE;
	case (BYTE)PacketType::E_S_ANS_PLAYER_MOVE:
		return new PK_S_ANS_PLAYER_MOVE;
	case (BYTE)PacketType::E_C_REQ_BULLET_SHOOT:
		return new PK_C_REQ_BULLET_SHOOT;
	case (BYTE)PacketType::E_S_ANS_BULLET_SHOOT:
		return new PK_S_ANS_BULLET_SHOOT;
	case (BYTE)PacketType::E_C_REQ_COLLISION_CHECK:
		return new PK_C_REQ_COLLISION_CHECK;
	case (BYTE)PacketType::E_S_ANS_COLLISION_CHECK:
		return new PK_S_ANS_COLLISION_CHECK;
	case (BYTE)PacketType::E_C_REQ_CHAT_INPUT:
		return new PK_C_REQ_CHAT_INPUT;
	case (BYTE)PacketType::E_S_ANS_CHAT_OUTPUT:
		return new PK_S_ANS_CHAT_OUTPUT;
	case (BYTE)PacketType::E_C_REQ_EXIT_ROOM:
		return new PK_C_REQ_EXIT_ROOM;
	case (BYTE)PacketType::E_S_ANS_EXIT_ROOM:
		return new PK_S_ANS_EXIT_ROOM;
	}

	return nullptr;
}