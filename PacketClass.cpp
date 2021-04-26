#include "pch.h"

#include "PacketClass.h"

void Packet::Encoding(Stream& stream)
{
	stream << (BYTE)this->type();
}

void Packet::Decoding(Stream& stream)
{}

//---------------------------------------------------------------------
// PK_C_REQ_EXIT
//---------------------------------------------------------------------
PacketType PK_C_REQ_EXIT::type()
{
	return PacketType::E_C_REQ_EXIT;
}

void PK_C_REQ_EXIT::Encoding(Stream& stream)
{
	stream << (BYTE)this->type();
	stream << _uid;
}

void PK_C_REQ_EXIT::Decoding(Stream& stream)
{
	stream >> &_uid;
}

//---------------------------------------------------------------------
// PK_S_ANS_EXIT
//---------------------------------------------------------------------
PacketType PK_S_ANS_EXIT::type()
{
	return PacketType::E_S_ANS_EXIT;
}

void PK_S_ANS_EXIT::Encoding(Stream& stream)
{
	stream << (BYTE)this->type();
	stream << _uid;
}

void PK_S_ANS_EXIT::Decoding(Stream& stream)
{
	stream >> &_uid;
}

//---------------------------------------------------------------------
// PK_C_REQ_REGISTER
//---------------------------------------------------------------------
PacketType PK_C_REQ_REGISTER::type()
{
	return PacketType::E_C_REQ_REGISTER;
}

void PK_C_REQ_REGISTER::Encoding(Stream& stream)
{
	stream << (BYTE)this->type();
	stream << _charName;
	stream << _password;
}

void PK_C_REQ_REGISTER::Decoding(Stream& stream)
{
	stream >> &_charName;
	stream >> &_password;
}

//---------------------------------------------------------------------
// PK_S_ANS_REGISTER_FAIL
//---------------------------------------------------------------------
PacketType PK_S_ANS_REGISTER_FAIL::type()
{
	return PacketType::E_S_ANS_REGISTER_FAIL;
}

void PK_S_ANS_REGISTER_FAIL::Encoding(Stream& stream)
{
	stream << (BYTE)this->type();
	stream << _charName;
}

void PK_S_ANS_REGISTER_FAIL::Decoding(Stream& stream)
{
	stream >> &_charName;
}

//---------------------------------------------------------------------
// PK_S_ANS_REGISTER_SUCESS
//---------------------------------------------------------------------
PacketType PK_S_ANS_REGISTER_SUCCESS::type()
{
	return PacketType::E_S_ANS_REGISTER_SUCCESS;
}

void PK_S_ANS_REGISTER_SUCCESS::Encoding(Stream& stream)
{
	stream << (BYTE)this->type();
}

void PK_S_ANS_REGISTER_SUCCESS::Decoding(Stream& stream)
{}

//---------------------------------------------------------------------
// PK_C_REQ_LOGIN
//---------------------------------------------------------------------
PacketType PK_C_REQ_LOGIN::type()
{
	return PacketType::E_C_REQ_LOGIN;
}

void PK_C_REQ_LOGIN::Encoding(Stream& stream)
{
	stream << (BYTE)this->type();
	stream << _charName;
	stream << _password;
}

void PK_C_REQ_LOGIN::Decoding(Stream& stream)
{
	stream >> &_charName;
	stream >> &_password;
}

//---------------------------------------------------------------------
// PK_S_ANS_LOGIN_FAIL
//---------------------------------------------------------------------
PacketType PK_S_ANS_LOGIN_FAIL::type()
{
	return PacketType::E_S_ANS_LOGIN_FAIL;
}

void PK_S_ANS_LOGIN_FAIL::Encoding(Stream& stream)
{
	stream << (BYTE)this->type();
	stream << _charName;
}

void PK_S_ANS_LOGIN_FAIL::Decoding(Stream& stream)
{
	stream >> &_charName;
}

//---------------------------------------------------------------------
// PK_S_ANS_LOGIN_SUCCESS
//---------------------------------------------------------------------
PacketType PK_S_ANS_LOGIN_SUCCESS::type()
{
	return PacketType::E_S_ANS_LOGIN_SUCCESS;
}

void PK_S_ANS_LOGIN_SUCCESS::Encoding(Stream& stream)
{
	stream << (BYTE)this->type();
	stream << _uid;
	stream << _charName;
}

void PK_S_ANS_LOGIN_SUCCESS::Decoding(Stream& stream)
{
	stream >> &_uid;
	stream >> &_charName;
}

//---------------------------------------------------------------------
// PK_C_NOTIFY_LOGIN_CLIENT
//---------------------------------------------------------------------
PacketType PK_C_NOTIFY_LOGIN_CLIENT::type()
{
	return PacketType::E_C_NOTIFY_LOGIN_CLIENT;
}

void PK_C_NOTIFY_LOGIN_CLIENT::Encoding(Stream& stream)
{
	stream << (BYTE)this->type();
	stream << _uid;
	stream << _charName;
}

void PK_C_NOTIFY_LOGIN_CLIENT::Decoding(Stream& stream)
{
	stream >> &_uid;
	stream >> &_charName;
}

//---------------------------------------------------------------------
// PK_S_NOTIFY_LOGIN_CLIENT
//---------------------------------------------------------------------
PacketType PK_S_NOTIFY_LOGIN_CLIENT::type()
{
	return PacketType::E_S_NOTIFY_LOGIN_CLIENT;
}

void PK_S_NOTIFY_LOGIN_CLIENT::Encoding(Stream& stream)
{
	stream << (BYTE)this->type();
	stream << _check;
	stream << _uid;
	stream << _charName;
}

void PK_S_NOTIFY_LOGIN_CLIENT::Decoding(Stream& stream)
{
	stream >> &_check;
	stream >> &_uid;
	stream >> &_charName;
}

//---------------------------------------------------------------------
// PK_C_REQ_CONNECT_ROOM
//---------------------------------------------------------------------
PacketType PK_C_REQ_CONNECT_ROOM::type()
{
	return PacketType::E_C_REQ_CONNECT_ROOM;
}

void PK_C_REQ_CONNECT_ROOM::Encoding(Stream& stream)
{
	stream << (BYTE)this->type();
	stream << _roomNumber;
	stream << _uid;
	stream << _charName;
}

void PK_C_REQ_CONNECT_ROOM::Decoding(Stream& stream)
{
	stream >> &_roomNumber;
	stream >> &_uid;
	stream >> &_charName;
}

//---------------------------------------------------------------------
// PK_S_ANS_CONNECT_ROOM
//---------------------------------------------------------------------
PacketType PK_S_ANS_CONNECT_ROOM::type()
{
	return PacketType::E_S_ANS_CONNECT_ROOM;
}

void PK_S_ANS_CONNECT_ROOM::Encoding(Stream& stream)
{
	stream << (BYTE)this->type();
	stream << _roomNumber;
	stream << _uid;
	stream << _charName;
}

void PK_S_ANS_CONNECT_ROOM::Decoding(Stream& stream)
{
	stream >> &_roomNumber;
	stream >> &_uid;
	stream >> &_charName;
}

//---------------------------------------------------------------------
// PK_S_ANS_CONNECT_FAIL_ROOM
//---------------------------------------------------------------------
PacketType PK_S_ANS_CONNECT_FAIL_ROOM::type()
{
	return PacketType::E_S_ANS_CONNECT_FAIL_ROOM;
}

void PK_S_ANS_CONNECT_FAIL_ROOM::Encoding(Stream& stream)
{
	stream << (BYTE)this->type();
	stream << _uid;
}

void PK_S_ANS_CONNECT_FAIL_ROOM::Decoding(Stream& stream)
{
	stream >> &_uid;
}

//---------------------------------------------------------------------
// PK_C_NOTIFY_CONNECT_ROOM_CLIENT
//---------------------------------------------------------------------
PacketType PK_C_NOTIFY_CONNECT_ROOM_CLIENT::type()
{
	return PacketType::E_C_NOTIFY_CONNECT_ROOM_CLIENT;
}

void PK_C_NOTIFY_CONNECT_ROOM_CLIENT::Encoding(Stream& stream)
{
	stream << (BYTE)this->type();
	stream << _roomNumber;
	stream << _uid;
	stream << _charName;
}

void PK_C_NOTIFY_CONNECT_ROOM_CLIENT::Decoding(Stream& stream)
{
	stream >> &_roomNumber;
	stream >> &_uid;
	stream >> &_charName;
}

//---------------------------------------------------------------------
// PK_S_NOTIFY_CONNECT_ROOM_CLIENT
//---------------------------------------------------------------------
PacketType PK_S_NOTIFY_CONNECT_ROOM_CLIENT::type()
{
	return PacketType::E_S_NOTIFY_CONNECT_ROOM_CLIENT;
}

void PK_S_NOTIFY_CONNECT_ROOM_CLIENT::Encoding(Stream& stream)
{
	stream << (BYTE)this->type();
	stream << _check;
	stream << _roomNumber;
	stream << _uid;
	stream << _charName;
}

void PK_S_NOTIFY_CONNECT_ROOM_CLIENT::Decoding(Stream& stream)
{
	stream >> &_check;
	stream >> &_roomNumber;
	stream >> &_uid;
	stream >> &_charName;
}

//---------------------------------------------------------------------
// PK_C_REQ_PLAYER_MOVE
//---------------------------------------------------------------------
PacketType PK_C_REQ_PLAYER_MOVE::type()
{
	return PacketType::E_C_REQ_PLAYER_MOVE;
}

void PK_C_REQ_PLAYER_MOVE::Encoding(Stream& stream)
{
	stream << (BYTE)this->type();
	stream << _roomNumber;
	stream << _uid;
	stream << _charName;
	stream << _xpos;
	stream << _ypos;
	stream << _zpos;
	stream << _xrot;
	stream << _yrot;
	stream << _zrot;
}

void PK_C_REQ_PLAYER_MOVE::Decoding(Stream& stream)
{
	stream >> &_roomNumber;
	stream >> &_uid;
	stream >> &_charName;
	stream >> &_xpos;
	stream >> &_ypos;
	stream >> &_zpos;
	stream >> &_xrot;
	stream >> &_yrot;
	stream >> &_zrot;
}

//---------------------------------------------------------------------
// PK_S_ANS_PLAYER_MOVE
//---------------------------------------------------------------------
PacketType PK_S_ANS_PLAYER_MOVE::type()
{
	return PacketType::E_S_ANS_PLAYER_MOVE;
}

void PK_S_ANS_PLAYER_MOVE::Encoding(Stream& stream)
{
	stream << (BYTE)this->type();
	stream << _uid;
	stream << _charName;
	stream << _xpos;
	stream << _ypos;
	stream << _zpos;
	stream << _xrot;
	stream << _yrot;
	stream << _zrot;
}

void PK_S_ANS_PLAYER_MOVE::Decoding(Stream& stream)
{
	stream >> &_uid;
	stream >> &_charName;
	stream >> &_xpos;
	stream >> &_ypos;
	stream >> &_zpos;
	stream >> &_xrot;
	stream >> &_yrot;
	stream >> &_zrot;
}

//---------------------------------------------------------------------
// PK_C_REQ_BULLET_SHOOT
//---------------------------------------------------------------------
PacketType PK_C_REQ_BULLET_SHOOT::type()
{
	return PacketType::E_C_REQ_BULLET_SHOOT;
}

void PK_C_REQ_BULLET_SHOOT::Encoding(Stream& stream)
{
	stream << (BYTE)this->type();
	stream << _roomNumber;
	stream << _uid;
}

void PK_C_REQ_BULLET_SHOOT::Decoding(Stream& stream)
{
	stream >> &_roomNumber;
	stream >> &_uid;
}

//---------------------------------------------------------------------
// PK_S_ANS_BULLET_SHOOT
//---------------------------------------------------------------------
PacketType PK_S_ANS_BULLET_SHOOT::type()
{
	return PacketType::E_S_ANS_BULLET_SHOOT;
}

void PK_S_ANS_BULLET_SHOOT::Encoding(Stream& stream)
{
	stream << (BYTE)this->type();
	stream << _uid;
}

void PK_S_ANS_BULLET_SHOOT::Decoding(Stream& stream)
{
	stream >> &_uid;
}

//---------------------------------------------------------------------
// PK_C_REQ_COLLISION_CHECK
//---------------------------------------------------------------------
PacketType PK_C_REQ_COLLISION_CHECK::type()
{
	return PacketType::E_C_REQ_COLLISION_CHECK;
}

void PK_C_REQ_COLLISION_CHECK::Encoding(Stream& stream)
{
	stream << (BYTE)this->type();
	stream << _roomNumber;
	stream << _uid;
}

void PK_C_REQ_COLLISION_CHECK::Decoding(Stream& stream)
{
	stream >> &_roomNumber;
	stream >> &_uid;
}

//---------------------------------------------------------------------
// PK_C_REQ_COLLISION_CHECK
//---------------------------------------------------------------------
PacketType PK_S_ANS_COLLISION_CHECK::type()
{
	return PacketType::E_S_ANS_COLLISION_CHECK;
}

void PK_S_ANS_COLLISION_CHECK::Encoding(Stream& stream)
{
	stream << (BYTE)this->type();
	stream << _uid;
}

void PK_S_ANS_COLLISION_CHECK::Decoding(Stream& stream)
{
	stream >> &_uid;
}

//---------------------------------------------------------------------
// PK_C_REQ_CHAT_INPUT
//---------------------------------------------------------------------
PacketType PK_C_REQ_CHAT_INPUT::type()
{
	return PacketType::E_C_REQ_CHAT_INPUT;
}

void PK_C_REQ_CHAT_INPUT::Encoding(Stream& stream)
{
	stream << (BYTE)this->type();
	stream << _roomNumber;
	stream << _charName;
	stream << _text;
}
void PK_C_REQ_CHAT_INPUT::Decoding(Stream& stream)
{
	stream >> &_roomNumber;
	stream >> &_charName;
	stream >> &_text;
}

//---------------------------------------------------------------------
// PK_S_ANS_CHAT_OUTPUT
//---------------------------------------------------------------------
PacketType PK_S_ANS_CHAT_OUTPUT::type()
{
	return PacketType::E_S_ANS_CHAT_OUTPUT;
}

void PK_S_ANS_CHAT_OUTPUT::Encoding(Stream& stream)
{
	stream << (BYTE)this->type();
	stream << _charName;
	stream << _text;
}
void PK_S_ANS_CHAT_OUTPUT::Decoding(Stream& stream)
{
	stream >> &_charName;
	stream >> &_text;
}

//---------------------------------------------------------------------
// PK_C_REQ_EXIT_ROOM
//---------------------------------------------------------------------
PacketType PK_C_REQ_EXIT_ROOM::type()
{
	return PacketType::E_C_REQ_EXIT_ROOM;
}

void PK_C_REQ_EXIT_ROOM::Encoding(Stream& stream)
{
	stream << (BYTE)this->type();
	stream << _roomNumber;
	stream << _uid;
	stream << _charName;
}

void PK_C_REQ_EXIT_ROOM::Decoding(Stream& stream)
{
	stream >> &_roomNumber;
	stream >> &_uid;
	stream >> &_charName;
}

//---------------------------------------------------------------------
// PK_S_ANS_EXIT_ROOM
//---------------------------------------------------------------------
PacketType PK_S_ANS_EXIT_ROOM::type()
{
	return PacketType::E_S_ANS_EXIT_ROOM;
}

void PK_S_ANS_EXIT_ROOM::Encoding(Stream& stream)
{
	stream << (BYTE)this->type();
	stream << _uid;
	stream << _charName;
}

void PK_S_ANS_EXIT_ROOM::Decoding(Stream& stream)
{
	stream >> &_uid;
	stream >> &_charName;
}