#include "pch.h"

#include "Room.h"

Room::Room(BYTE roomNumber)
	:_roomNumber(roomNumber) //, _userCount(0)
	,_check(FALSE)
{
	InitializeSRWLock(&_lock);
}

Room::~Room()
{}

void Room::AddUser(User* user)
{
	AcquireSRWLockExclusive(&_lock);
	_userMap.insert(std::make_pair(user->_uid, user));
	ReleaseSRWLockExclusive(&_lock);
}

void Room::PopUser(WORD uid)
{
	// User객체로 지우는 것은 순회가 필요하므로 key로 지우는 것이 낫다

	if (_userMap.empty())
	{
		return;
	}

	AcquireSRWLockExclusive(&_lock);
	_userMap.erase(uid);
	// --accountId; 감소가 일어나면 겹칠 가능성이 존재..
	ReleaseSRWLockExclusive(&_lock);
}

BYTE Room::GetRoomNumber()
{
	return _roomNumber;
}

User* Room::GetUser(WORD uid)
{
	AcquireSRWLockShared(&_lock);
	User* user = _userMap.find(uid)->second;
	ReleaseSRWLockShared(&_lock);

	return user;
}

std::unordered_map<WORD, User*>& Room::GetUserMap()
{
	return _userMap;
}