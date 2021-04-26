#include "pch.h"

#include "Package.h"

Package::Package(Session* session, Packet* packet)
	:_session(session), _packet(packet)
{}

Package::~Package()
{
	_session = nullptr;
	delete _packet;
}