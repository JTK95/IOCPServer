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
	// Ž���ӵ� O(1)�̹Ƿ� unordered_map�� Ȱ����
	std::unordered_map<WORD, User*> _usermap;
	SRWLOCK _lock;
};
