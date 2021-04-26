#include "pch.h"

#include "Task.h"

TaskNode::TaskNode(Work* workObject, int freqSec, int durationSec)
	:_workObject(workObject), _freqSec(freqSec), _durationSec(durationSec)
{
	this->NextTick();
}

TaskNode::~TaskNode()
{
	delete _workObject;
}

void TaskNode::NextTick()
{
	_nextTick = Clock::GetInstance()->SystemTick() + (time_t)_freqSec;
}

//-----------------------------------------------------------------------
// task가 만기 됐나 안 됐나 확인
//-----------------------------------------------------------------------
bool TaskNode::expired()
{
	if (_workObject == nullptr)
	{
		return true;
	}

	if (_durationSec != TICK_INFINITY)
	{
		if (_durationSec < Clock::GetInstance()->SystemTick())
		{
			return true;
		}
	}

	return false;
}

//-----------------------------------------------------------------------
// tick마다 업데이트 되는 함수
//-----------------------------------------------------------------------
void TaskNode::tick()
{
	if (_nextTick < Clock::GetInstance()->SystemTick())
	{
		_workObject->tick();
		this->NextTick();
	}
}

//------------------------------------------------------------------------//
Task::Task(int id)
	:_id(id)
{}

Task::~Task()
{
	for (auto& node : _taskList)
	{
		if (node != nullptr)
			delete node;
	}
}

void Task::add(TaskNode* taskNode)
{
	std::lock_guard<std::mutex> guard(_lock);
	_taskList.push_back(taskNode);
}

void Task::remove(TaskNode* taskNode)
{
	std::lock_guard<std::mutex> guard(_lock);

	for (auto iter = _taskList.begin(); iter != _taskList.end(); ++iter)
	{
		if (*iter == taskNode)
		{
			_taskList.erase(iter);
			break;
		}
	}
}

//------------------------------------------------------------------------
// 쓰레드가 돌 동안 돌아가는 또 하나의 흐름 (작은 main 함수)
// 리스트에서 끝난 taskNode들 제거 및 갱신
//------------------------------------------------------------------------
void Task::process()
{
	while (!_shutdown)
	{
		if (_taskList.empty())
		{
			continue;
		}

		//----------------------------------------------------------------------
		// lock_guard는 lock과 달리 lock과 unlock을 호출해준다
		// 따라서 lock, unlock 사용이 자신이 없으면 lock_gurad를 사용하자
		// 장점: 임계영역에서 return문을 만나 unlock을 하지 못한 상황들을 해결
		//----------------------------------------------------------------------
		std::lock_guard<std::mutex> guard(_lock);

		// 삭제할 task
		std::vector<TaskNode*> deleteNode;
		
		//----------------------------------------------------------------------
		// taskList 순회
		// task 만기 됐는지 확인 후 만기 됐으면 deleteNode의 삽입
		//----------------------------------------------------------------------
		for (auto& task : _taskList)
		{
			TaskNode* taskNode = task;
			if (taskNode->expired())
			{
				deleteNode.push_back(taskNode);
				continue;
			}
			taskNode->tick();
		}

		// 만기된 task들 삭제
		for (auto node : deleteNode)
		{
			this->remove(node);
		}

		Sleep(15);
	}
}

//------------------------------------------------------------------------
// 쓰레드 생성
// 사실상 지금 내 서버는 taskThread가 필요가없네..
//------------------------------------------------------------------------
void Task::Update()
{
	_taskThread = MAKE_THREAD(Task, process);
}


//-----------------------------------------------------------------------//
TaskManager::TaskManager()
{}

TaskManager::~TaskManager()
{}

TaskManager* TaskManager::GetInstance()
{
	static TaskManager taskManager;

	return &taskManager;
}

//-------------------------------------------------------------------------
// config파일로 _threadCount 만큼 task생성
//-------------------------------------------------------------------------
void TaskManager::Initialize(TiXmlDocument* config)
{
	TiXmlElement* root = config->FirstChildElement("App")->FirstChildElement("Task");
	if (!root)
	{
		puts("TaskManager::Initialize() not exist task setting");
		//SLog(L"@ not exist task setting");

		return;
	}

	// ThreadCount(5) 만큼 멤버인 _threadCount 대입
	TiXmlElement* elem = root->FirstChildElement("ThreadCount");
	sscanf_s(elem->GetText(), "%d", &_threadCount);

	for (int i = 0; i < _threadCount; i++)
	{
		Task* task = new Task(i);
		_taskPool.push_back(task);
		task->Update();
	}

	printf_s("TaskManager::Initialize() Create task Thread Count : [%d]\n\n", _threadCount);
	//SLog(L"* task thread, [%d] maked", _threadCount);
}

void TaskManager::release()
{
	for (auto task : _taskPool)
	{
		delete task;
	}
}

//-------------------------------------------------------------------------
// workObject를 _taskPool에 추가해줌
//-------------------------------------------------------------------------
void TaskManager::add(Work* workObject, int freqSec, int durationSec)
{
	const int minimalThreadCount = 1;
	if (_threadCount < minimalThreadCount)
	{
		return;
	}

	static int nodeCount = 0;

	TaskNode* node = new TaskNode(workObject, freqSec, durationSec);
	int index = nodeCount % _threadCount;
	Task* task = _taskPool[index];
	task->add(node);

	++nodeCount;
}