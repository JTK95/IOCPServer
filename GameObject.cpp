#include "pch.h"
#include "pch.h"

#include "GameObject.h"

Object::Object()
{}

Object::~Object()
{}

NameObject::NameObject()
{}

NameObject::~NameObject()
{}

std::wstring& NameObject::name()
{
	return _name;
}

void NameObject::setName(std::wstring name)
{
	_name = name;
}

GameObject::GameObject()
{
	_position.x = 0;
	_position.y = 0;
	_direction = 0.0f;
}

GameObject::~GameObject()
{
	this->free();
}