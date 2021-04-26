#include "pch.h"

#include "User.h"

User::User(WORD uid, std::string charName, Session* session)
    : _uid(uid), _charName(charName), _session(session), _roomNumber(0)
{}

User::~User()
{}

Session* User::GetSession()
{
    return _session;
}

WORD User::GetUID()
{
    return _uid;
}