#include "pch.h"

#include "PacketProcess.h"

CRITICAL_SECTION lock;

PacketProcess::PacketProcess()
	:ContentsProcess()
{
	InitializeCriticalSection(&lock);

	this->RegistSubPacketFunc();

	//-----------------------------------------------------------------------
	// 룸 4개 생성
	// 4번째 룸은 더미클라이언트 테스트 룸
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

	// 룸 삭제
	for (int i = 0; i < 4; ++i)
	{
		Room* room = new Room(i + 1);
		RoomManager::GetInstance()->PopRoom(room);
	}
}

//CRITICAL_SECTION lock;

void PacketProcess::RegistSubPacketFunc()
{
	//InitializeCriticalSection(&lock); // 더미클라이언트 번호부여시 동기화를 위한 객체
#define INSERT_PACKET_PROCESS(type)  _runFuncTable.insert(std::make_pair(PacketType::E_##type, &PacketProcess::##type))

	// ContentsProcess의 run에서 콜백함수로 불러다씀
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
// 회원가입 기능
//-------------------------------------------------------------------------------
void PacketProcess::C_REQ_REGISTER(Session* session, Packet* rowPacket)
{
	PK_C_REQ_REGISTER* packet = (PK_C_REQ_REGISTER*)rowPacket;

	std::wstring wcharName;
	std::wstring wpassword;

	// 아스키코드 -> 유니코드 (디비가 유니코드 받음)
	wcharName.assign(packet->_charName.begin(), packet->_charName.end());
	wpassword.assign(packet->_password.begin(), packet->_password.end());
	
	/*printf_s("ID: %ws\n", wid.c_str());
	printf_s("PW: %ws\n", wpassword.c_str());*/
	
	//Database::GetInstance()->Delete(wcharName);
	//Database::GetInstance()->Show();

	//-------------------------------------------------------------------------------------
	// 회원가입 SUCCESS or FAIL
	//-------------------------------------------------------------------------------------
	if (!Database::GetInstance()->Match(wcharName, wpassword, DB_MATCH_TYPE::REGISTER_TYPE))
	{
		// 중복되지 않은 정보이면 DB의 정보 삽입
		Database::GetInstance()->Insert(wcharName, wpassword);

		// 회원가입 성공 패킷 송신
		PK_S_ANS_REGISTER_SUCCESS retPacket;
		session->SendPacket(&retPacket);

		puts(" 회원가입 성공!");
	}
	else
	{
		// 회원가입 실패 패킷 송신
		PK_S_ANS_REGISTER_FAIL retPacket;
		retPacket._charName = packet->_charName;
		session->SendPacket(&retPacket);
		puts("회원가입 실패..");
	}
}

//-------------------------------------------------------------------------------
// 로그인 기능
//-------------------------------------------------------------------------------
void PacketProcess::C_REQ_LOGIN(Session* session, Packet* rowPacket)
{
	PK_C_REQ_LOGIN* packet = (PK_C_REQ_LOGIN*)rowPacket;

	std::wstring wcharName;
	std::wstring wpassword;

	// 아스키코드 -> 유니코드
	wcharName.assign(packet->_charName.begin(), packet->_charName.end());
	wpassword.assign(packet->_password.begin(), packet->_password.end());

	//-------------------------------------------------------------------------------------
	// 로그인 SUCCESS or FAIL
	//-------------------------------------------------------------------------------------
	if (!Database::GetInstance()->Match(wcharName, wpassword, DB_MATCH_TYPE::LOGIN_TYPE))
	{
		// 로그인 실패 패킷 송신
		PK_S_ANS_LOGIN_FAIL retPacket;
		retPacket._charName = packet->_charName;
		session->SendPacket(&retPacket);
		puts("로그인 실패..");
	}
	else
	{
		// 유저 생성 및 추가
		User* user = UserManager::GetInstance()->CreateUser(packet->_charName, session);
		UserManager::GetInstance()->AddUser(user);

		// 로그인 성공 패킷 송신
		PK_S_ANS_LOGIN_SUCCESS retPacket;
		retPacket._uid = user->_uid;
		retPacket._charName = user->_charName;
		session->SendPacket(&retPacket);

		puts("로그인 성공!");
	}
}

//-------------------------------------------------------------------------------
// InGame 기능
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
	// <유저 맵 순회>
	// 타 유저 정보 --> 내 자신
	// 내 정보 --> 타 유저
	//------------------------------------------------------------------------------------
	for (auto& iter : UserManager::GetInstance()->GetUserMap())
	{
		User* otherUser = iter.second;
		if (otherUser == nullptr)
		{
			printf_s("C_NOTIFY_LOGIN_CLIENT otherUser is not exist userAccountId : %ud  \n", otherUser->GetUID());

			return;
		}

		// 내 차례 건너뛰기
		if (user == otherUser)
		{
			continue;
		}
		
		// 내 정보 --> 타 유저
		PK_S_NOTIFY_LOGIN_CLIENT mySessionInfo;
		mySessionInfo._check = true;
		mySessionInfo._uid = user->_uid;
		mySessionInfo._charName = user->_charName;
		otherUser->GetSession()->SendPacket(&mySessionInfo);

		// 타 유저 정보 --> 내 자신
		PK_S_NOTIFY_LOGIN_CLIENT otherSessionInfo;
		otherSessionInfo._check = false;
		otherSessionInfo._uid = otherUser->_uid;
		otherSessionInfo._charName = otherUser->_charName;
		user->GetSession()->SendPacket(&otherSessionInfo);
	}

	printf_s("[%s] 님이 로비에 입장하였습니다.\n", user->_charName.c_str());
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
		// 방 꽉참 응답
		PK_S_ANS_CONNECT_FAIL_ROOM retPacket;
		retPacket._uid = packet->_uid;
		user->GetSession()->SendPacket(&retPacket);

		return;
	}

	user->_roomNumber = packet->_roomNumber;
	room->AddUser(user);

	// 다른 유저 접속까지 대기하므로 반환
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
	// 클라에서 동시에 패킷 수신되므로 동기화를 위해 락을 걸어두자
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
	// 동시에 커넥트 하므로 쓰레드가 2개가 들어온다...
	// 플래그를 하나두고 하나의 쓰레드는 반환해버리자
	//-------------------------------------------------------------------------
	if (room->_check == TRUE)
	{
		LeaveCriticalSection(&lock);
		return;
	}

	// 현재 방 안에 있는 유저 순회
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
	// 로비에서 다시 입장했을때 고유번호인 _uid는 잘 식별된다...뭔가문제지
	//------------------------------------------------------------------------------
	//printf_s("accountId: %llu\n", packet->_uid);
	
	//------------------------------------------------------------------------------------
	// <유저 맵 순회>
	// 내 정보 --> 타 유저
	//------------------------------------------------------------------------------------
	for (auto& iter : room->GetUserMap())
	{
		User* otherUser = iter.second;
		if (otherUser == nullptr)
		{
			printf_s("C_REQ_PLAYER_MOVE otherUser is not exist userAccountId : %ud  \n", otherUser->GetUID());

			return;
		}

		// 내 차례 건너뛰기
		if (user == otherUser)
		{
			continue;
		}

		// 내 정보 --> 타 유저
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
	// <유저 맵 순회>
	// 내 정보 --> 타 유저
	//------------------------------------------------------------------------------------
	for (auto& iter : room->GetUserMap())
	{
		User* otherUser = iter.second;
		if (otherUser == nullptr)
		{
			printf_s("C_REQ_BULLET_SHOT otherUser is not exist userAccountId : %ud  \n", otherUser->GetUID());

			return;
		}

		// 내 차례 건너뛰기
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
// 채팅 기능
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
		
		// 내 정보 --> 타 유저
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

	printf_s("[%s] 님이 Room을 나가셨습니다.\n", packet->_charName.c_str());
}