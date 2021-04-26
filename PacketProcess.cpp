#include "pch.h"

#include "PacketProcess.h"

CRITICAL_SECTION lock;

PacketProcess::PacketProcess()
	:ContentsProcess()
{
	InitializeCriticalSection(&lock);

	this->RegistSubPacketFunc();

	//-----------------------------------------------------------------------
	// �� 4�� ����
	// 4��° ���� ����Ŭ���̾�Ʈ �׽�Ʈ ��
	//-----------------------------------------------------------------------
	for (int i = 0; i < 4; ++i)
	{
		Room* room = new Room(i + 1);
		RoomManager::GetInstance()->AddRoom(room);
	}
}

PacketProcess::~PacketProcess()
{
	ContentsProcess::~ContentsProcess();

	// �� ����
	for (int i = 0; i < 4; ++i)
	{
		Room* room = new Room(i + 1);
		RoomManager::GetInstance()->PopRoom(room);
	}
}

//CRITICAL_SECTION lock;

void PacketProcess::RegistSubPacketFunc()
{
	//InitializeCriticalSection(&lock); // ����Ŭ���̾�Ʈ ��ȣ�ο��� ����ȭ�� ���� ��ü
#define INSERT_PACKET_PROCESS(type)  _runFuncTable.insert(std::make_pair(PacketType::E_##type, &PacketProcess::##type))

	// ContentsProcess�� run���� �ݹ��Լ��� �ҷ��پ�
	INSERT_PACKET_PROCESS(C_REQ_REGISTER);
	INSERT_PACKET_PROCESS(C_REQ_LOGIN);
	INSERT_PACKET_PROCESS(C_NOTIFY_LOGIN_CLIENT);
	INSERT_PACKET_PROCESS(C_REQ_CONNECT_ROOM);
	INSERT_PACKET_PROCESS(C_NOTIFY_CONNECT_ROOM_CLIENT);
	INSERT_PACKET_PROCESS(C_REQ_PLAYER_MOVE);
	INSERT_PACKET_PROCESS(C_REQ_BULLET_SHOOT);
	INSERT_PACKET_PROCESS(C_REQ_COLLISION_CHECK);
	INSERT_PACKET_PROCESS(C_REQ_CHAT_INPUT);
	INSERT_PACKET_PROCESS(C_REQ_EXIT_ROOM);
}

//-------------------------------------------------------------------------------
// ȸ������ ���
//-------------------------------------------------------------------------------
void PacketProcess::C_REQ_REGISTER(Session* session, Packet* rowPacket)
{
	PK_C_REQ_REGISTER* packet = (PK_C_REQ_REGISTER*)rowPacket;

	std::wstring wcharName;
	std::wstring wpassword;

	// �ƽ�Ű�ڵ� -> �����ڵ� (��� �����ڵ� ����)
	wcharName.assign(packet->_charName.begin(), packet->_charName.end());
	wpassword.assign(packet->_password.begin(), packet->_password.end());
	
	/*printf_s("ID: %ws\n", wid.c_str());
	printf_s("PW: %ws\n", wpassword.c_str());*/
	
	//Database::GetInstance()->Delete(wcharName);
	//Database::GetInstance()->Show();

	//-------------------------------------------------------------------------------------
	// ȸ������ SUCCESS or FAIL
	//-------------------------------------------------------------------------------------
	if (!Database::GetInstance()->Match(wcharName, wpassword, DB_MATCH_TYPE::REGISTER_TYPE))
	{
		// �ߺ����� ���� �����̸� DB�� ���� ����
		Database::GetInstance()->Insert(wcharName, wpassword);

		// ȸ������ ���� ��Ŷ �۽�
		PK_S_ANS_REGISTER_SUCCESS retPacket;
		session->SendPacket(&retPacket);

		puts(" ȸ������ ����!");
	}
	else
	{
		// ȸ������ ���� ��Ŷ �۽�
		PK_S_ANS_REGISTER_FAIL retPacket;
		retPacket._charName = packet->_charName;
		session->SendPacket(&retPacket);
		puts("ȸ������ ����..");
	}
}

//-------------------------------------------------------------------------------
// �α��� ���
//-------------------------------------------------------------------------------
void PacketProcess::C_REQ_LOGIN(Session* session, Packet* rowPacket)
{
	PK_C_REQ_LOGIN* packet = (PK_C_REQ_LOGIN*)rowPacket;

	std::wstring wcharName;
	std::wstring wpassword;

	// �ƽ�Ű�ڵ� -> �����ڵ�
	wcharName.assign(packet->_charName.begin(), packet->_charName.end());
	wpassword.assign(packet->_password.begin(), packet->_password.end());

	//-------------------------------------------------------------------------------------
	// �α��� SUCCESS or FAIL
	//-------------------------------------------------------------------------------------
	if (!Database::GetInstance()->Match(wcharName, wpassword, DB_MATCH_TYPE::LOGIN_TYPE))
	{
		// �α��� ���� ��Ŷ �۽�
		PK_S_ANS_LOGIN_FAIL retPacket;
		retPacket._charName = packet->_charName;
		session->SendPacket(&retPacket);
		puts("�α��� ����..");
	}
	else
	{
		// ���� ���� �� �߰�
		User* user = UserManager::GetInstance()->CreateUser(packet->_charName, session);
		UserManager::GetInstance()->AddUser(user);

		// �α��� ���� ��Ŷ �۽�
		PK_S_ANS_LOGIN_SUCCESS retPacket;
		retPacket._uid = user->_uid;
		retPacket._charName = user->_charName;
		session->SendPacket(&retPacket);

		puts("�α��� ����!");
	}
}

//-------------------------------------------------------------------------------
// InGame ���
//-------------------------------------------------------------------------------
void PacketProcess::C_NOTIFY_LOGIN_CLIENT(Session* session, Packet* rowPacket)
{
	PK_C_NOTIFY_LOGIN_CLIENT* packet = (PK_C_NOTIFY_LOGIN_CLIENT*)rowPacket;
	
	User* user = UserManager::GetInstance()->Find(packet->_uid);
	if (user == nullptr)
	{
		printf_s("C_NOTIFY_LOGIN_CLIENT user is not exist userAccountId : %ud  \n", user->GetUID());
		
		delete packet;		
		return;
	}

	//------------------------------------------------------------------------------------
	// <���� �� ��ȸ>
	// Ÿ ���� ���� --> �� �ڽ�
	// �� ���� --> Ÿ ����
	//------------------------------------------------------------------------------------
	for (auto& iter : UserManager::GetInstance()->GetUserMap())
	{
		User* otherUser = iter.second;
		if (otherUser == nullptr)
		{
			printf_s("C_NOTIFY_LOGIN_CLIENT otherUser is not exist userAccountId : %ud  \n", otherUser->GetUID());

			return;
		}

		// �� ���� �ǳʶٱ�
		if (user == otherUser)
		{
			continue;
		}
		
		// �� ���� --> Ÿ ����
		PK_S_NOTIFY_LOGIN_CLIENT mySessionInfo;
		mySessionInfo._check = true;
		mySessionInfo._uid = user->_uid;
		mySessionInfo._charName = user->_charName;
		otherUser->GetSession()->SendPacket(&mySessionInfo);

		// Ÿ ���� ���� --> �� �ڽ�
		PK_S_NOTIFY_LOGIN_CLIENT otherSessionInfo;
		otherSessionInfo._check = false;
		otherSessionInfo._uid = otherUser->_uid;
		otherSessionInfo._charName = otherUser->_charName;
		user->GetSession()->SendPacket(&otherSessionInfo);
	}

	printf_s("[%s] ���� �κ� �����Ͽ����ϴ�.\n", user->_charName.c_str());
}

void PacketProcess::C_REQ_CONNECT_ROOM(Session* session, Packet* rowPacket)
{
	PK_C_REQ_CONNECT_ROOM* packet = (PK_C_REQ_CONNECT_ROOM*)rowPacket;

	User* user = UserManager::GetInstance()->Find(packet->_uid);
	if (user == nullptr)
	{
		printf_s("C_REQ_CONNECT_ROOM user is not exist userAccountId : %ud  \n", user->GetUID());

		return;
	}

	Room* room = RoomManager::GetInstance()->GetRoom(packet->_roomNumber);

	if (room->_userMap.size() == 2)
	{
		// �� ���� ����
		PK_S_ANS_CONNECT_FAIL_ROOM retPacket;
		retPacket._uid = packet->_uid;
		user->GetSession()->SendPacket(&retPacket);

		return;
	}

	user->_roomNumber = packet->_roomNumber;
	room->AddUser(user);

	// �ٸ� ���� ���ӱ��� ����ϹǷ� ��ȯ
	if (room->GetUserMap().size() < 2)
	{
		return;
	}
	
	for (auto& iter : room->GetUserMap())
	{
		User* roomUser = iter.second;
		if (roomUser == nullptr)
		{
			printf_s("C_REQ_CONNECT_ROOM roomUser is not exist userAccountId : %ud  \n", roomUser->GetUID());

			return;
		}

		PK_S_ANS_CONNECT_ROOM ansPacket;
		ansPacket._roomNumber = roomUser->_roomNumber;
		ansPacket._uid = roomUser->_uid;
		ansPacket._charName = roomUser->_charName;
		roomUser->GetSession()->SendPacket(&ansPacket);
	}
}

void PacketProcess::C_NOTIFY_CONNECT_ROOM_CLIENT(Session* session, Packet* rowPacket)
{
	// Ŭ�󿡼� ���ÿ� ��Ŷ ���ŵǹǷ� ����ȭ�� ���� ���� �ɾ����
	EnterCriticalSection(&lock);
	PK_C_NOTIFY_CONNECT_ROOM_CLIENT* packet = (PK_C_NOTIFY_CONNECT_ROOM_CLIENT*)rowPacket;
	
	User* user = UserManager::GetInstance()->Find(packet->_uid);
	if (user == nullptr)
	{
		printf_s("C_NOTIFY_CONNECT_ROOM_CLIENT user is not exist userAccountId : %ud  \n", user->GetUID());

		delete packet;
		LeaveCriticalSection(&lock);
		return;
	}

	Room* room = RoomManager::GetInstance()->GetRoom(packet->_roomNumber);

	//-------------------------------------------------------------------------
	// ���ÿ� Ŀ��Ʈ �ϹǷ� �����尡 2���� ���´�...
	// �÷��׸� �ϳ��ΰ� �ϳ��� ������� ��ȯ�ع�����
	//-------------------------------------------------------------------------
	if (room->_check == TRUE)
	{
		LeaveCriticalSection(&lock);
		return;
	}

	// ���� �� �ȿ� �ִ� ���� ��ȸ
	for(auto& iter : room->GetUserMap())
	{
		User* otherUser = iter.second;
		if (otherUser == nullptr)
		{
			printf_s("C_NOTIFY_CONNECT_ROOM_CLIENT otherUser is not exist userAccountId : %ud  \n", otherUser->GetUID());

			LeaveCriticalSection(&lock);
			return;
		}

		if (user == otherUser)
		{
			continue;
		}
		
		PK_S_NOTIFY_CONNECT_ROOM_CLIENT mySessionInfo;
		mySessionInfo._check = true;
		mySessionInfo._roomNumber = user->_roomNumber;
		mySessionInfo._uid = user->_uid;
		mySessionInfo._charName = user->_charName;
		otherUser->GetSession()->SendPacket(&mySessionInfo);

		PK_S_NOTIFY_CONNECT_ROOM_CLIENT otherSessionInfo;
		otherSessionInfo._check = false;
		otherSessionInfo._roomNumber = otherUser->_roomNumber;
		otherSessionInfo._uid = otherUser->_uid;
		otherSessionInfo._charName = otherUser->_charName;
		user->GetSession()->SendPacket(&otherSessionInfo);
	}

	room->_check = TRUE;

	LeaveCriticalSection(&lock);
}

void PacketProcess::C_REQ_PLAYER_MOVE(Session* session, Packet* rowPacket)
{
	PK_C_REQ_PLAYER_MOVE* packet = (PK_C_REQ_PLAYER_MOVE*)rowPacket;

	User* user = UserManager::GetInstance()->Find(packet->_uid);
	if (user == nullptr)
	{
		printf_s("C_REQ_PLAYER_MOVE user is not exist userAccountId : %ud  \n", user->GetUID());

		return;
	}

	Room* room = RoomManager::GetInstance()->GetRoom(packet->_roomNumber);
	PK_S_ANS_PLAYER_MOVE retPacket;
	//------------------------------------------------------------------------------
	// �κ񿡼� �ٽ� ���������� ������ȣ�� _uid�� �� �ĺ��ȴ�...����������
	//------------------------------------------------------------------------------
	//printf_s("accountId: %llu\n", packet->_uid);
	
	//------------------------------------------------------------------------------------
	// <���� �� ��ȸ>
	// �� ���� --> Ÿ ����
	//------------------------------------------------------------------------------------
	for (auto& iter : room->GetUserMap())
	{
		User* otherUser = iter.second;
		if (otherUser == nullptr)
		{
			printf_s("C_REQ_PLAYER_MOVE otherUser is not exist userAccountId : %ud  \n", otherUser->GetUID());

			return;
		}

		// �� ���� �ǳʶٱ�
		if (user == otherUser)
		{
			continue;
		}

		// �� ���� --> Ÿ ����
		//PK_S_ANS_PLAYER_MOVE retPacket;
		retPacket._uid = user->_uid;
		retPacket._charName = packet->_charName;
		retPacket._xpos = packet->_xpos;
		retPacket._ypos = packet->_ypos;
		retPacket._zpos = packet->_zpos;
		retPacket._xrot = packet->_xrot;
		retPacket._yrot = packet->_yrot;
		retPacket._zrot = packet->_zrot;
		otherUser->GetSession()->SendPacket(&retPacket);
	}

	//printf_s("user [%s] position [x: %f]  [y: %f]  [z: %f]\n", retPacket._charName.c_str(), retPacket._xpos, retPacket._ypos, retPacket._zpos);
}

void PacketProcess::C_REQ_BULLET_SHOOT(Session* session, Packet* rowPacket)
{
	PK_C_REQ_BULLET_SHOOT* packet = (PK_C_REQ_BULLET_SHOOT*)rowPacket;

	User* user = UserManager::GetInstance()->Find(packet->_uid);
	if (user == nullptr)
	{
		printf_s("C_REQ_BULLET_SHOT user is not exist userAccountId : %ud  \n", user->GetUID());

		return;
	}

	Room* room = RoomManager::GetInstance()->GetRoom(packet->_roomNumber);

	//------------------------------------------------------------------------------------
	// <���� �� ��ȸ>
	// �� ���� --> Ÿ ����
	//------------------------------------------------------------------------------------
	for (auto& iter : room->GetUserMap())
	{
		User* otherUser = iter.second;
		if (otherUser == nullptr)
		{
			printf_s("C_REQ_BULLET_SHOT otherUser is not exist userAccountId : %ud  \n", otherUser->GetUID());

			return;
		}

		// �� ���� �ǳʶٱ�
		if (user == otherUser)
		{
			continue;
		}

		PK_S_ANS_BULLET_SHOOT retPacket;
		retPacket._uid = user->_uid;
		otherUser->GetSession()->SendPacket(&retPacket);
	}
	printf_s("Shoot Packet!\n");
}

void PacketProcess::C_REQ_COLLISION_CHECK(Session* session, Packet* rowPacket)
{
	PK_C_REQ_COLLISION_CHECK* packet = (PK_C_REQ_COLLISION_CHECK*)rowPacket;

	User* user = UserManager::GetInstance()->Find(packet->_uid);
	if (user == nullptr)
	{
		printf_s("C_REQ_COLLISION_CHECK user is not exist userAccountId : %ud  \n", user->GetUID());

		return;
	}

	Room* room = RoomManager::GetInstance()->GetRoom(packet->_roomNumber);

	for (auto& iter : room->GetUserMap())
	{
		User* otherUser = iter.second;
		if (user == nullptr)
		{
			printf_s("C_REQ_COLLISION_CHECK otherUser is not exist userAccountId : %ud  \n", otherUser->GetUID());

			return;
		}

		if (user == otherUser)
		{
			continue;
		}

		PK_S_ANS_COLLISION_CHECK retPacket;
		retPacket._uid = user->_uid;
		otherUser->GetSession()->SendPacket(&retPacket);
	}
	printf_s("Collision Packet\n");
}

//-------------------------------------------------------------------------------
// ä�� ���
//-------------------------------------------------------------------------------
void PacketProcess::C_REQ_CHAT_INPUT(Session* session, Packet* rowPacket)
{
	PK_C_REQ_CHAT_INPUT* packet = (PK_C_REQ_CHAT_INPUT*)rowPacket;

	Room* room = RoomManager::GetInstance()->GetRoom(packet->_roomNumber);
	for (auto& iter : room->GetUserMap())
	{
		User* otherUser = iter.second;
		if (otherUser == nullptr)
		{
			printf_s("C_REQ_CHAT_INPUT otherUser is not exist userAccountId : %ud  \n", otherUser->GetUID());

			return;
		}
		
		// �� ���� --> Ÿ ����
		PK_S_ANS_CHAT_OUTPUT mySessionInfo;
		mySessionInfo._charName = packet->_charName;
		mySessionInfo._text = packet->_text;
		otherUser->GetSession()->SendPacket(&mySessionInfo);
	}

	printf_s("[ROOM %d] [%s] : %s\n", packet->_roomNumber, packet->_charName.c_str(), packet->_text.c_str());
}

void PacketProcess::C_REQ_EXIT_ROOM(Session* session, Packet* rowPacket)
{
	PK_C_REQ_EXIT_ROOM* packet = (PK_C_REQ_EXIT_ROOM*)rowPacket;

	User* user = UserManager::GetInstance()->Find(packet->_uid);
	if (user == nullptr)
	{
		printf_s("C_REQ_EXIT_ROOM user is not exist userAccountId : %ud  \n", user->GetUID());

		return;
	}

	Room* room = RoomManager::GetInstance()->GetRoom(packet->_roomNumber);
	room->PopUser(user->_uid);

	for (auto& iter : room->GetUserMap())
	{
		User* otherUser = iter.second;
		if (otherUser == nullptr)
		{
			printf_s("C_REQ_EXIT_ROOM otherUser is not exist userAccountId : %ud  \n", otherUser->GetUID());

			return;
		}

		if (user == otherUser)
		{
			continue;
		}

		PK_S_ANS_EXIT_ROOM mySessionInfo;
		mySessionInfo._uid = user->_uid;
		mySessionInfo._charName = user->_charName;
		otherUser->GetSession()->SendPacket(&mySessionInfo);

		/*PK_S_ANS_EXIT_ROOM otherSessionInfo;
		otherSessionInfo._accountId = otherUser->_accountId;
		otherSessionInfo._charName = otherUser->_charName;
		user->GetSession()->SendPacket(&otherSessionInfo);*/
	}
	room->_check = FALSE;

	printf_s("[%s] ���� Room�� �����̽��ϴ�.\n", packet->_charName.c_str());
}