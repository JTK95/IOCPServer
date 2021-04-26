#pragma once

#define MAX_PACKET_THREAD 64

//------------------------------------------------------------
// ��Ű���� ť�� �װ�,
// ���� �����忡�� ���� ������ �����尡 ������ �տ� ť�� �ִ� 
// ��Ű���� �����ٰ� _runFuncTable�� ��ϵ� �Լ� �����ͷ� ����
//------------------------------------------------------------
class ContentsProcess
{
public:
	ContentsProcess();
	virtual ~ContentsProcess();

	void PutPackage(Package* package);
	
	void RegistDefaultPacketFunc();
	
	// PacketProcessŬ�������� Ȱ��
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