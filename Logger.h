#pragma once

// �α� ��ũ��
#define SLog(arg, ...)     SystemLog::GetInstance()->log(arg, __VA_ARGS__);
#define SErrLog(arg, ...)  SystemLog::GetInstance()->log(arg, __VA_ARGS__); ::ExitProcess(0);

class LogBase
{
public:
	LogBase() {}
	virtual ~LogBase() {}
	//virtual void Initialize() {}
	//virtual void unInitialize() {}
	virtual void log(const WCHAR* logStr) = 0;
};

class LogPrintf : public LogBase
{
public:
	LogPrintf();
	virtual ~LogPrintf();

	virtual void log(const WCHAR* logStr);
};

class LogFile : public LogBase
{
public:
	LogFile(TiXmlDocument* config);
	virtual ~LogFile();

	void Initialize(WCHAR* logFileName);
	virtual void log(const WCHAR* logStr);

private:
	std::wfstream _fs;
	std::wstring _fileName;
};

//-------------------------------------------------------------------
// �α� ���� ��ü
//-------------------------------------------------------------------
class LogWriter
{
public:
	LogWriter();
	virtual ~LogWriter();

	void SetLogger(LogBase* base, const WCHAR* logPrefix);
	LogBase* logger() const;

	void log(const WCHAR* fmt, ...);
	void log(const WCHAR* fmt, char* args);

private:
	LogBase* _base;
	std::wstring _prefix;
};

//-------------------------------------------------------------------
// ���� �ý��� �α� Singleton Class
//-------------------------------------------------------------------
class SystemLog
{
public:
	static SystemLog* GetInstance();

	void Initialize(TiXmlDocument* config);
	void log(const WCHAR* fmt, ...);

private:
	SystemLog();
	virtual ~SystemLog();

	LogWriter _logWrite;
};