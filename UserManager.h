#pragma once

//-----------------------------------------------------------------------
// Singleton Class
//-----------------------------------------------------------------------
class UserManager
{
public:
	volatile static SHORT UserKey;
	static UserManager* GetInstance();

	void AddUser(User* user);

	void PopUser(WORD uid);

	User* CreateUser(std::string charName, Session* session);

	User* Find(WORD uid);

	// getFunc
	std::unordered_map<WORD, User*>& GetUserMap();

private:
	UserManager();
	~UserManager();

private:
	// 탐색속도 O(1)이므로 unordered_map을 활용함
	std::unordered_map<WORD, User*> _usermap;
	SRWLOCK _lock;
};
