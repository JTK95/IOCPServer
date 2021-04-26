#pragma once
#include "pch.h"

//--------------------------------------------------------------------
// ��Ŷ Ÿ�� (1Byte)
// Ȯ�强�� ����Ѵ��ص�
// ���� �� ������Ʈ�� 0~255 ������ ��Ŷ�� �ʰ����� ���� �� ����
// �׷��Ƿ� -127~ ������ char�� ���� 
// unsigned char�� BYTE�� ��������
//--------------------------------------------------------------------
enum class PacketType : BYTE
{
	// default
	E_C_REQ_EXIT = 1,
	E_S_ANS_EXIT = 2,

	// ȸ������, �α���
	E_C_REQ_REGISTER = 3,
	E_S_ANS_REGISTER_FAIL = 4,
	E_S_ANS_REGISTER_SUCCESS = 5,
	E_C_REQ_LOGIN = 6,
	E_S_ANS_LOGIN_FAIL = 7,
	E_S_ANS_LOGIN_SUCCESS = 8,

	// ���� ���� Ȯ��
	E_C_NOTIFY_LOGIN_CLIENT = 9,
	E_S_NOTIFY_LOGIN_CLIENT = 10,

	// �κ�
	E_C_REQ_CONNECT_ROOM = 11,
	E_S_ANS_CONNECT_ROOM = 12,
	E_S_ANS_CONNECT_FAIL_ROOM = 13,
	E_C_NOTIFY_CONNECT_ROOM_CLIENT = 14,
	E_S_NOTIFY_CONNECT_ROOM_CLIENT = 15,

	// �÷��̾� �̵�
	E_C_REQ_PLAYER_MOVE = 16,
	E_S_ANS_PLAYER_MOVE = 17,

	// �Ѿ� ó��
	E_C_REQ_BULLET_SHOOT = 18,
	E_S_ANS_BULLET_SHOOT = 19,

	// �浹 üũ
	E_C_REQ_COLLISION_CHECK = 20,
	E_S_ANS_COLLISION_CHECK = 21,

	// ä��
	E_C_REQ_CHAT_INPUT = 22,
	E_S_ANS_CHAT_OUTPUT = 23,

	E_C_REQ_EXIT_ROOM = 24,
	E_S_ANS_EXIT_ROOM = 25
};