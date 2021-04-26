#include "pch.h"

#include "Clock.h"

//-----------------------------------------------------------------------
// ��� _serverStartTick�� ����ð� ����
//-----------------------------------------------------------------------
Clock::Clock()
	:_serverStartTick(this->SystemTick())
{}

Clock::~Clock()
{}

Clock* Clock::GetInstance()
{
	static Clock clockManager;

	return &clockManager;
}

HRESULT Clock::Initialize()
{
	return S_OK;
}

void Clock::release()
{
}

std::wstring Clock::tickToStr(time_t tick, const WCHAR* fmt)
{
	std::array<WCHAR, 128> timeStr;

	tm time;
	localtime_s(&time, &tick);
	wcsftime(timeStr.data(), timeStr.size(), fmt, &time);

	return timeStr.data();
}

time_t Clock::strToTick(std::wstring str, const WCHAR* fmt)
{
	int year = 0;
	int month = 0;
	int day = 0;
	int hour = 0;
	int minute = 0;
	int second = 0;

	swscanf_s(str.c_str(), fmt, &year, &month, &day, &hour, &minute, &second);

	// ��, ��, ��, ��, ��, ��
	tm time = { second, minute, hour, day, month - 1, year - 1900 };

	return mktime(&time);
}

time_t Clock::ServerStartTick() const
{
	return _serverStartTick;
}

//-----------------------------------------------------------------------------
// ���� �ý��� �ð� ��ȯ
//-----------------------------------------------------------------------------
time_t Clock::SystemTick()
{
	return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
}

std::wstring Clock::NowTime(const WCHAR* fmt)
{
	return this->tickToStr(this->SystemTick(), fmt);
}

std::wstring Clock::NowTimeWithMilliSec(const WCHAR* fmt)
{
#if 0 //���� ���
	//����ð�
	timePoint now = system_clock::now();
	//����ð��� tick_t����(���� ����)
	timePoint oldSecond = system_clock::from_time_t(this->systemTick());

	duration<double> milliSecond = now - oldSecond;
	array<WCHAR, SIZE_8> milliStr;
	snwprintf(milliStr, L"%03d", (int)(milliSecond.count() * 1000));
#else
	//���ػ� ���� �ð�
	std::chrono::high_resolution_clock::time_point point = std::chrono::high_resolution_clock::now();
	//���ػ� ���� �ð��� millisec���� ��ȯ
	std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(point.time_since_epoch());

	std::chrono::seconds s = std::chrono::duration_cast<std::chrono::seconds>(ms);
	time_t t = s.count();
	size_t fractionalSeconds = ms.count() % 1000;
	std::array<WCHAR, 8> milliStr;
	_snwprintf_s(milliStr.data(), milliStr.size(), _TRUNCATE, L"%03d", (int)(fractionalSeconds));
#endif

	std::wstring timeString = this->tickToStr(this->SystemTick(), fmt);
	timeString += L".";
	timeString += milliStr.data();
	return timeString;
}

std::wstring Clock::today()
{
	return this->tickToStr(this->SystemTick(), DATE_FORMAT);
}

//wstring CClock::tomorrow()
//{
//
//}
//
//wstring CClock::yesterday()
//{
//
//}

DayOfTheWeek Clock::todayTheWeek()
{
	tm time;
	time_t tick = this->SystemTick();
	localtime_s(&time, &tick);

	return (DayOfTheWeek)time.tm_mday;
}