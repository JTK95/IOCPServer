#pragma once

class User
{
public:
	User(WORD uid, std::string charName, Session* session);
	~User();

public:
	Session* GetSession();
	WORD GetUID();

    std::string _charName;
	WORD _uid;
	BYTE _roomNumber;

private:
	Session* _session;
};