#pragma once

class SystemReport : public Work
{
public:
	SystemReport() {};
	~SystemReport() {};

	void tick()
	{
		// TODO.. MONITERING
	}
};

class ServerClass
{
public:
	ServerClass();
	~ServerClass();

	HRESULT Initialize();
	void Update();
	void release();
	
private:
	void ServerProcess();
};

