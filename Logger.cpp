#include "pch.h"

#include "Logger.h"

LogPrintf::LogPrintf()
{
	puts("Log create : printf log mode");
}

LogPrintf::~LogPrintf()
{}

void LogPrintf::log(const WCHAR* logStr)
{
	printf_s("%ws\n", logStr);
}

//-------------------------------------------------------------------
// LogFile
//-------------------------------------------------------------------
LogFile::LogFile(TiXmlDocument* config)
{
	TiXmlElement* root = config->FirstChildElement("App")->FirstChildElement("Log");
	TiXmlElement* elem = root->FirstChildElement("Path");

	std::array<WCHAR, _MAX_PATH> logFilePath;
	MultiByteToWideChar
	(
		CP_ACP,
		0,
		(char*)elem->GetText(),
		-1,
		logFilePath.data(),
		logFilePath.max_size() - 1
	);
	
	printf_s("Log create : [ws]file log mode\n\n", logFilePath.data());
	this->Initialize(logFilePath.data());
}

LogFile::~LogFile()
{
	_fs.close();
	_fs.clear();

	size_t found = _fileName.find(L".log");
	if (found == std::wstring::npos)
	{
		return;
	}

	// TODO.. �ڿ� �α������� ������ �����, ����ð��� �����̸� �ڿ� �ٿ��ش�
	std::wstring closeFileName = _fileName.substr(0, found);
	closeFileName += Clock::GetInstance()->NowTime(L"_Y%m%d-%H%M%S.log");//(L"_Y%m%d-%H%M%S.log");
	_wrename(_fileName.c_str(), closeFileName.c_str());
}

void LogFile::Initialize(WCHAR* logFileName)
{
	_fileName = logFileName;
	_fs.open(logFileName, std::ios::out | std::ios::trunc);
	if (_fs.bad())
	{
		puts("logfile error, file open fail");
		assert(false);
	}
}

void LogFile::log(const WCHAR* logStr)
{
	printf_s("%ws\n", logStr);

	_fs << logStr;
	_fs.flush();
}

//---------------------------------------------------------------
// CLogWriter �α� ���� ��ü
//---------------------------------------------------------------
LogWriter::LogWriter()
	:_base(nullptr)
{}

LogWriter::~LogWriter()
{
	_prefix.clear();

	delete _base;
}

void LogWriter::SetLogger(LogBase* base, const WCHAR* logPrefix)
{
	_prefix.clear();
	_prefix = logPrefix;

	if (_base)
	{
		LogBase* old = _base;
		_base = nullptr;
		
		delete old;
	}

	_base = base;
}

LogBase* LogWriter::logger() const
{
	return _base;
}

void LogWriter::log(const WCHAR* fmt, ...)
{
	char* args;
	va_start(args, fmt);

	this->log(fmt, args);

	va_end(args);
}

void LogWriter::log(const WCHAR* fmt, char* args)
{
	// ���� �ð� �ֱ�
	std::wstring logMessage = Clock::GetInstance()->NowTimeWithMilliSec();
	std::thread::id threadId = GET_CURRENT_THREAD_ID();

	logMessage += L"\t";

	// ������ ���� �ֱ�
	Thread* thread = ThreadManager::GetInstance()->at(threadId);
	if (thread)
	{
		logMessage += thread->name();
	}
	else
	{
		logMessage += _prefix;
	}

	std::array<WCHAR, 8 * 2> threadIdStr;
	_snwprintf_s(threadIdStr.data(), threadIdStr.size(), _TRUNCATE, L"0x%X", threadId);

	logMessage += L":";
	logMessage += threadIdStr.data();
	logMessage += L"\t";

	std::array<WCHAR, 1024> logStr;
	vswprintf_s(logStr.data(), logStr.size(), fmt, args);

	logMessage += logStr.data();
	logMessage += L"\n";
	_base->log((WCHAR*)logMessage.c_str());
}

//--------------------------------------------------------------------
SystemLog::SystemLog()
{
	TiXmlDocument config;
	
	if (!loadConfig(&config))
	{
		puts("!!! have not config file");
		exit(0);
		return;
	}
	this->Initialize(&config);
}

SystemLog::~SystemLog()
{}

SystemLog* SystemLog::GetInstance()
{
	static SystemLog logManager;

	return &logManager;
}

void SystemLog::Initialize(TiXmlDocument* config)
{
	// config ������ �α� ã��
	TiXmlElement* root = config->FirstChildElement("App")->FirstChildElement("Log");
	if (!root)
	{
		puts("not exist log setting");
		LogBase* base = new LogPrintf;
		_logWrite.SetLogger(base, L"testServer");

		return;
	}

	// Path ã��
	TiXmlElement* elem = root->FirstChildElement("Path");

	// prefix ã�� prefix����
	std::array<WCHAR, 256> tmp;
	elem = root->FirstChildElement("Prefix");
	MultiByteToWideChar
	(
		CP_ACP,
		0,
		(char*)elem->GetText(),
		-1,
		tmp.data(),
		tmp.max_size() - 1
	);

	std::wstring prefix = tmp.data();

	// Log Ÿ�� ã��
	LogBase* base;
	elem = root->FirstChildElement("Type");

	// WithFile Ÿ���̸� LogFile ���
	const char* type = (char*)elem->GetText();
	if (!strcmp(type, "WithFile"))
	{
		base = new LogFile(config);
	}
	// �ƴϸ� LogPrintf ���
	else
	{
		base = new LogPrintf;
	}

	// �α� base Ŭ������ prefix����
	_logWrite.SetLogger(base, prefix.c_str());
}

void SystemLog::log(const WCHAR* fmt, ...)
{
	char* args;
	va_start(args, fmt);
	_logWrite.log(fmt, args);
	va_end(args);
}
