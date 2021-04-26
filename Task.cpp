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
// task�� ���� �Ƴ� �� �Ƴ� Ȯ��
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
// tick���� ������Ʈ �Ǵ� �Լ�
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
// �����尡 �� ���� ���ư��� �� �ϳ��� �帧 (���� main �Լ�)
// ����Ʈ���� ���� taskNode�� ���� �� ����
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
		// lock_guard�� lock�� �޸� lock�� unlock�� ȣ�����ش�
		// ���� lock, unlock ����� �ڽ��� ������ lock_gurad�� �������
		// ����: �Ӱ迵������ return���� ���� unlock�� ���� ���� ��Ȳ���� �ذ�
		//----------------------------------------------------------------------
		std::lock_guard<std::mutex> guard(_lock);

		// ������ task
		std::vector<TaskNode*> deleteNode;
		
		//----------------------------------------------------------------------
		// taskList ��ȸ
		// task ���� �ƴ��� Ȯ�� �� ���� ������ deleteNode�� ����
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

		// ����� task�� ����
		for (auto node : deleteNode)
		{
			this->remove(node);
		}

		Sleep(15);
	}
}

//------------------------------------------------------------------------
// ������ ����
// ��ǻ� ���� �� ������ taskThread�� �ʿ䰡����..
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
// config���Ϸ� _threadCount ��ŭ task����
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

	// ThreadCount(5) ��ŭ ����� _threadCount ����
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
// workObject�� _taskPool�� �߰�����
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