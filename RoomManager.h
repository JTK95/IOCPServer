#pragma once

//-------------------------------------------------------------------
// Singleton Class
//-------------------------------------------------------------------
class RoomManager
{
public:
	volatile static __int64 roomKey;
	static RoomManager* GetInstance();

public:
	void AddRoom(Room* room);
	void PopRoom(Room* room);
	Room* GetRoom(BYTE roomNumber);

private:
	RoomManager();
	~RoomManager();

	SRWLOCK _lock;

	// roomNumber, Room
	std::unordered_map<BYTE, Room*> _roomMap;
};

