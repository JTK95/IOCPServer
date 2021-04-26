#pragma once

#define TICK_INFINITY 0

class TaskNode
{
public:
	TaskNode(Work* workObject, int freqSec, int durationSec);
	~TaskNode();

	void NextTick();

	// task가 만기 됐나 안 됐나 확인
	bool expired();

	// tick마다 업데이트 되는 함수
	void tick();

private:
	Work* _workObject;
	int _freqSec;
	int _durationSec;

	time_t _nextTick;
};

class Task
{
public:
	Task(int id);
	~Task();

	void add(TaskNode* taskNode);
	void remove(TaskNode* taskNode);

	void process();
	void Update();

private:
	std::mutex _lock;
	std::vector<TaskNode*> _taskList;
	Thread* _taskThread;
	int _id;
};

//--------------------------------------------------------------
// Singleton Class
//--------------------------------------------------------------
class TaskManager
{
public:
	static TaskManager* GetInstance();

	void Initialize(TiXmlDocument* config);
	void release();

	void add(Work* workObject, int freqSec, int durationSec);

private:
	TaskManager();
	~TaskManager();

	int _threadCount;
	std::vector<Task*>  _taskPool;
};