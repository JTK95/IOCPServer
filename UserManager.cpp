#include "pch.h"

#include "UserManager.h"

volatile SHORT UserManager::UserKey = 0;

UserManager::UserManager()
{
	InitializeSRWLock(&_lock);
}

UserManager::~UserManager()
{}

UserManager* UserManager::GetInstance()
{
	static UserManager userManager;

	return &userManager;
}

void UserManager::AddUser(User* user)// &user
{
	AcquireSRWLockExclusive(&_lock);
	_usermap.insert(std::make_pair(user->GetUID(), user));
	ReleaseSRWLockExclusive(&_lock);
}

void UserManager::PopUser(WORD uid)
{
	AcquireSRWLockExclusive(&_lock);
	_usermap.erase(uid);
	ReleaseSRWLockExclusive(&_lock);
}

User* UserManager::CreateUser(std::string charName, Session* session)
{
	WORD uid = (WORD)InterlockedIncrement16(&UserKey);

	return std::move(new User(uid, charName, session));
}

User* UserManager::Find(WORD uid)
{
	AcquireSRWLockShared(&_lock);

	// find()는 iterator가 반환값
	std::unordered_map<WORD, User*>::iterator iter = _usermap.find(uid);
	
	ReleaseSRWLockShared(&_lock);

	if (iter != _usermap.end())
	{
		return iter->second;
	}
	else
	{
		return nullptr;
	}
}

std::unordered_map<WORD, User*>& UserManager::GetUserMap()
{	
	return _usermap;
}
