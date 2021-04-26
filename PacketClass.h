#pragma once

class Packet
{
public:
	virtual PacketType type() = 0;
	virtual void Encoding(Stream& stream);
	virtual void Decoding(Stream& stream);
};

class PK_C_REQ_EXIT : public Packet
{
public:
	PacketType type();

	virtual void Encoding(Stream& stream);
	virtual void Decoding(Stream& stream);

public:
	WORD _uid;
};

class PK_S_ANS_EXIT : public Packet
{
public:
	PacketType type();

	virtual void Encoding(Stream& stream);
	virtual void Decoding(Stream& stream);

public:
	WORD _uid;
};

class PK_C_REQ_REGISTER : public Packet
{
public:
	PacketType type();

	virtual void Encoding(Stream& stream);
	virtual void Decoding(Stream& stream);

public:
	std::string _charName;
	std::string _password;
};

class PK_S_ANS_REGISTER_FAIL : public Packet
{
public:
	PacketType type();

	virtual void Encoding(Stream& stream);
	virtual void Decoding(Stream& stream);

public:
	std::string _charName;
};

class PK_S_ANS_REGISTER_SUCCESS : public Packet
{
public:
	PacketType type();

	virtual void Encoding(Stream& stream);
	virtual void Decoding(Stream& stream);
};

class PK_C_REQ_LOGIN : public Packet
{
public:
	PacketType type();

	virtual void Encoding(Stream& stream);
	virtual void Decoding(Stream& stream);

public:
	std::string _charName;
	std::string _password;
};

class PK_S_ANS_LOGIN_FAIL : public Packet
{
public:
	PacketType type();

	virtual void Encoding(Stream& stream);
	virtual void Decoding(Stream& stream);

public:
	std::string _charName;
};

class PK_S_ANS_LOGIN_SUCCESS : public Packet
{
public:
	PacketType type();

	virtual void Encoding(Stream& stream);
	virtual void Decoding(Stream& stream);

public:
	WORD _uid;
	std::string _charName;
};

class PK_C_NOTIFY_LOGIN_CLIENT : public Packet
{
public:
	PacketType type();

	virtual void Encoding(Stream& stream);
	virtual void Decoding(Stream& stream);

public:
	WORD _uid;
	std::string _charName;
};

class PK_S_NOTIFY_LOGIN_CLIENT : public Packet
{
public:
	PacketType type();

	virtual void Encoding(Stream& stream);
	virtual void Decoding(Stream& stream);

public:
	bool _check;

	WORD _uid;
	std::string _charName;
};

class PK_C_REQ_CONNECT_ROOM : public Packet
{
public:
	PacketType type();

	virtual void Encoding(Stream& stream);
	virtual void Decoding(Stream& stream);

public:
	BYTE _roomNumber;
	WORD _uid;
	std::string _charName;
};

class PK_S_ANS_CONNECT_ROOM : public Packet
{
public:
	PacketType type();

	virtual void Encoding(Stream& stream);
	virtual void Decoding(Stream& stream);

public:
	BYTE _roomNumber;
	WORD _uid;
	std::string _charName;
};

class PK_S_ANS_CONNECT_FAIL_ROOM : public Packet
{
public:
	PacketType type();

	virtual void Encoding(Stream& stream);
	virtual void Decoding(Stream& stream);

public:
	WORD _uid;
};

class PK_C_NOTIFY_CONNECT_ROOM_CLIENT : public Packet
{
public:
	PacketType type();

	virtual void Encoding(Stream& stream);
	virtual void Decoding(Stream& stream);

public:
	BYTE _roomNumber;
	WORD _uid;
	std::string _charName;
};

class PK_S_NOTIFY_CONNECT_ROOM_CLIENT : public Packet
{
public:
	PacketType type();

	virtual void Encoding(Stream& stream);
	virtual void Decoding(Stream& stream);

public:
	bool _check;
	BYTE _roomNumber;
	WORD _uid;
	std::string _charName;
};

class PK_C_REQ_PLAYER_MOVE : public Packet
{
public:
	PacketType type();
	
	virtual void Encoding(Stream& stream);
	virtual void Decoding(Stream& stream);

public:
	BYTE _roomNumber;
	WORD _uid;

	// 플레이어 이름
	std::string _charName;

	// 좌표
	float _xpos;
	float _ypos;
	float _zpos;
	
	// 회전
	float _xrot;
	float _yrot;
	float _zrot;
};

class PK_S_ANS_PLAYER_MOVE : public Packet
{
public:
	PacketType type();

	virtual void Encoding(Stream& stream);
	virtual void Decoding(Stream& stream);

public:
	WORD _uid;

	std::string _charName;

	float _xpos;
	float _ypos;
	float _zpos;

	float _xrot;
	float _yrot;
	float _zrot;
};

class PK_C_REQ_BULLET_SHOOT : public Packet
{
public:
	PacketType type();

	virtual void Encoding(Stream& stream);
	virtual void Decoding(Stream& stream);

public:
	BYTE _roomNumber;
	WORD _uid;
};

class PK_S_ANS_BULLET_SHOOT : public Packet
{
public:
	PacketType type();

	virtual void Encoding(Stream& stream);
	virtual void Decoding(Stream& stream);

public:
	WORD _uid;
};

class PK_C_REQ_COLLISION_CHECK : public Packet
{
public:
	PacketType type();

	virtual void Encoding(Stream& stream);
	virtual void Decoding(Stream& stream);

public:
	BYTE _roomNumber;
	WORD _uid;
};

class PK_S_ANS_COLLISION_CHECK : public Packet
{
public:
	PacketType type();

	virtual void Encoding(Stream& stream);
	virtual void Decoding(Stream& stream);

public:
	WORD _uid;
};

class PK_C_REQ_CHAT_INPUT : public Packet
{
public:
	PacketType type();

	virtual void Encoding(Stream& stream);
	virtual void Decoding(Stream& stream);

public:
	BYTE _roomNumber;
	std::string _charName;
	std::string _text;
};

class PK_S_ANS_CHAT_OUTPUT : public Packet
{
public:
	PacketType type();

	virtual void Encoding(Stream& stream);
	virtual void Decoding(Stream& stream);

public:
	std::string _charName;
	std::string _text;
};

class PK_C_REQ_EXIT_ROOM : public Packet
{
public:
	PacketType type();

	virtual void Encoding(Stream& stream);
	virtual void Decoding(Stream& stream);

public:
	BYTE _roomNumber;
	WORD _uid;
	std::string _charName;
};

class PK_S_ANS_EXIT_ROOM : public Packet
{
public:
	PacketType type();

	virtual void Encoding(Stream& stream);
	virtual void Decoding(Stream& stream);

public:
	WORD _uid;
	std::string _charName;
};