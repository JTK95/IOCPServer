#pragma once

const time_t TICK_MIN(60);
const time_t TICK_HOUR(TICK_MIN * 60);
const time_t TICK_DAY(TICK_HOUR * 24);

// ��ũ�� �Լ��� ������ ������ ���� inline ���
inline time_t TICK_TO_MIN(int& x) { return (x / TICK_MIN); }
inline time_t MIN_TO_TICK(int& x) { return (x * TICK_MIN); }
inline time_t TICK_TO_HOUR(int& x) { return (x / TICK_HOUR); }
inline time_t HOUR_TO_TICK(int& x) { return (x * TICK_HOUR); }
inline time_t TICK_TO_DAY(int& x) { return (x / TICK_DAY); }
inline time_t DAY_TO_TICK(int& x) { return (x * TICK_DAY); }

// ���� enum
enum class DayOfTheWeek
{
	DAY_SUNDAY,
	DAY_MONDAY,
	DAY_TUESDAY,
	DAY_WEDNESDAY,
	DAY_THURSDAY,
	DAY_FRIDAY,
	DAY_SATURDAY
};

// data ���� ��ũ��
#define DATETIME_FORMAT         L"D%Y-%m-%dT%H:%M:%S"
#define DATE_FORMAT             L"%Y-%m-%d"
#define TIME_FORMAT             L"%H:%M:%S"
#define DB_TIME_FORMAT          L"%4d-%2d-%2d %2d:%2d:%2d"

//---------------------------------------------------
// Singleton Class
//---------------------------------------------------
class Clock
{
public:
	static Clock* GetInstance();

	HRESULT Initialize();
	void release();

	time_t ServerStartTick() const; 
	time_t SystemTick();
	time_t strToTick(std::wstring str, const WCHAR* fmt = DB_TIME_FORMAT);
	
	std::wstring NowTime(const WCHAR* fmt = DATETIME_FORMAT);
	std::wstring NowTimeWithMilliSec(const WCHAR* fmt = DATETIME_FORMAT);

	std::wstring today();
//	wstring tomorrow();
//	wstring yesterday();

	DayOfTheWeek todayTheWeek();

private:
	Clock();
	~Clock();

	std::wstring tickToStr(time_t tick, const WCHAR* fmt = DATETIME_FORMAT);

	// ���� ���� �ð�
	time_t _serverStartTick;
};

