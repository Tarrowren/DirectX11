#include"TimerClass.h"

TimerClass::TimerClass() {

}

TimerClass::~TimerClass() {

}

bool TimerClass::Initialize() {
	//检查系统是否支持高性能定时器
	QueryPerformanceFrequency((LARGE_INTEGER*)&m_frequency);
	if (m_frequency == 0) {
		return false;
	}

	//找出频率计数器每毫秒记录多少次
	m_ticksPerMs = (float)(m_frequency / 1000);

	QueryPerformanceCounter((LARGE_INTEGER*)&m_startTime);

	return true;
}

void TimerClass::Frame() {
	INT64 currentTime;
	float timeDifference;

	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

	timeDifference = (float)(currentTime - m_startTime);

	m_frameTime = timeDifference / m_ticksPerMs;

	m_startTime = currentTime;

	return;
}

float TimerClass::GetTime() {
	return m_frameTime;
}