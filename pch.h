#pragma once

//----------------------------------------------------------
// Free-Compile Header
//----------------------------------------------------------
#define _WINSOCKAPI_
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <mstcpip.h>
#include <Ws2spi.h>
#include <Mstcpip.h>
#include <mswsock.h>
#include <Mmsystem.h>
#include <tchar.h>

#include <time.h>
#include <sql.h>
#include <sqlext.h>
#include <Windows.h>
#include <iostream>
#include <functional>
#include <thread>
#include <ctime>
#include <mutex>
#include <map>
#include <vector>
#include <unordered_map>
#include <list>
#include <array>
#include <chrono>
#include <ctime>
#include <string>
#include <strsafe.h>
#include <queue>
#include <fstream>
#include <psapi.h>
#include <unordered_map>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")

//#include "MemoryLeak.h"
#include "../IOCPServer/tinyXml/tinyxml.h"

#include "Config.h"
#include "Shutdown.h"
#include "GameObject.h"

#include "WinSock.h"
#include "Logger.h"
#include "Monitoring.h"
#include "Clock.h"

#include "Lock.h"
#include "Thread.h"
#include "ThreadJobQueue.h"

#include "Task.h"

#include "Stream.h"

#include "PacketHeader.h"
#include "PacketClass.h"
#include "PacketFactory.h"

#include "Session.h"
#include "SessionManager.h"
#include "IOCPSession.h"

#include "Package.h"

#include "PacketAnalyzer.h"

#include "ContentsProcess.h"

#include "Server.h"
#include "IOCPServer.h"

#include "Database.h"

// IOCP (Login, Lobby, InGame, Chatting)
#include "User.h"
#include "UserManager.h"
#include "Room.h"
#include "RoomManager.h"
#include "PacketProcess.h"
#include "ServerClass.h"
