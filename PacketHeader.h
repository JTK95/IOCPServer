#pragma once
#include "pch.h"

//--------------------------------------------------------------------
// 패킷 타입 (1Byte)
// 확장성을 고려한다해도
// 현재 내 프로젝트는 0~255 범위의 패킷이 초과되지 않을 것 같음
// 그러므로 -127~ 부터인 char형 보단 
// unsigned char인 BYTE로 지정하자
//--------------------------------------------------------------------
enum class PacketType : BYTE
{
	// default
	E_C_REQ_EXIT = 1,
	E_S_ANS_EXIT = 2,

	// 회원가입, 로그인
	E_C_REQ_REGISTER = 3,
	E_S_ANS_REGISTER_FAIL = 4,
	E_S_ANS_REGISTER_SUCCESS = 5,
	E_C_REQ_LOGIN = 6,
	E_S_ANS_LOGIN_FAIL = 7,
	E_S_ANS_LOGIN_SUCCESS = 8,

	// 유저 정보 확인
	E_C_NOTIFY_LOGIN_CLIENT = 9,
	E_S_NOTIFY_LOGIN_CLIENT = 10,

	// 로비
	E_C_REQ_CONNECT_ROOM = 11,
	E_S_ANS_CONNECT_ROOM = 12,
	E_S_ANS_CONNECT_FAIL_ROOM = 13,
	E_C_NOTIFY_CONNECT_ROOM_CLIENT = 14,
	E_S_NOTIFY_CONNECT_ROOM_CLIENT = 15,

	// 플레이어 이동
	E_C_REQ_PLAYER_MOVE = 16,
	E_S_ANS_PLAYER_MOVE = 17,

	// 총알 처리
	E_C_REQ_BULLET_SHOOT = 18,
	E_S_ANS_BULLET_SHOOT = 19,

	// 충돌 체크
	E_C_REQ_COLLISION_CHECK = 20,
	E_S_ANS_COLLISION_CHECK = 21,

	// 채팅
	E_C_REQ_CHAT_INPUT = 22,
	E_S_ANS_CHAT_OUTPUT = 23,

	E_C_REQ_EXIT_ROOM = 24,
	E_S_ANS_EXIT_ROOM = 25
};