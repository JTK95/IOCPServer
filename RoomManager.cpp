#include "pch.h"

#include "RoomManager.h"

volatile __int64 RoomManager::roomKey = 0;

RoomManager::RoomManager()
{
	InitializeSRWLock(&_lock);
}

RoomManager::~RoomManager()
{}

RoomManager* RoomManager::GetInstance()
{
	static RoomManager roomManager;

	return &roomManager;
}

void RoomManager::AddRoom(Room* room)
{
	AcquireSRWLockExclusive(&_lock);
	_roomMap.insert(std::make_pair(room->GetRoomNumber(), room));
	ReleaseSRWLockExclusive(&_lock);
}

void RoomManager::PopRoom(Room* room)
{
	if (_roomMap.empty())
	{
		return;
	}

	AcquireSRWLockExclusive(&_lock);
	_roomMap.erase(room->GetRoomNumber());
	ReleaseSRWLockExclusive(&_lock);

	delete room;
}

Room* RoomManager::GetRoom(BYTE roomNumber)
{
	if (_roomMap.empty())
	{
		return nullptr;
	}

	AcquireSRWLockShared(&_lock);
	Room* room = _roomMap.find(roomNumber)->second;
	ReleaseSRWLockShared(&_lock);

	return room;
}