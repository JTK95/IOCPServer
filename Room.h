#pragma once

class Room
{
public:
	Room(BYTE roomNumber);
	~Room();

	void AddUser(User* user);
	void PopUser(WORD uid);

	BYTE GetRoomNumber();
	User* GetUser(WORD uid);
	std::unordered_map<WORD, User*>& GetUserMap();

public:
	bool _check;
	BYTE _roomNumber;
	//int _userCount;
	SRWLOCK _lock;

	// _userCount, User
	std::unordered_map<WORD, User*> _userMap;
};

