#pragma once

#define MAX_PACKET_THREAD 64

//------------------------------------------------------------
// 패키지를 큐에 쌓고,
// 여러 쓰레드에서 실행 가능한 쓰레드가 나오면 앞에 큐에 있는 
// 패키지를 가져다가 _runFuncTable에 등록된 함수 포인터로 점프
//------------------------------------------------------------
class ContentsProcess
{
public:
	ContentsProcess();
	virtual ~ContentsProcess();

	void PutPackage(Package* package);
	
	void RegistDefaultPacketFunc();
	
	// PacketProcess클래스에서 활용
	virtual void RegistSubPacketFunc() = 0;

	static void C_REQ_EXIT(Session* session, Packet* rowPacket);

protected:

	typedef void(*RunFunc)(Session* session, Packet* packet);
	std::unordered_map<PacketType, RunFunc> _runFuncTable;

private:
	void Update(Package* package);
	void Execute();
	void Process();

	std::array<Thread*, MAX_PACKET_THREAD> _threadPool;
	ThreadJobQueue<Package*>* _packageQueue;
};