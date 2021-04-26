#pragma once

class Object
{
public:
	Object();
	~Object();

private:
	std::wstring _allocFile;
	int _allocLine;
};

class NameObject
{
public:
	NameObject();
	virtual ~NameObject();

	std::wstring& name();
	void setName(std::wstring name);

private:
	std::wstring _name;
};

class Work
{
public:
	Work() {}
	virtual ~Work() {}

	virtual void tick() = 0;
	virtual void wakeup() {};
	virtual void suspend() {};
	virtual void stop() {};
	virtual void start() {};
};

class GameObject : public NameObject, public Work
{
public:
	// 클래스 이름마다 소멸자 이름이 다르므로, free로 통일 시키자
	GameObject();
	virtual ~GameObject();

	virtual void Initialize() {};
	virtual void free() {};

private:
	POINT _position;
	float _direction;
};