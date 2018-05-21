#pragma once
#include<Windows.h>

class TimerClass {

public:

	TimerClass();
	~TimerClass();

	bool Initialize();
	void Frame();
	float GetTime();
	void StartTimer();
	void StopTimer();
	int GetTiming();

private:

	float m_frequency;
	INT64 m_startTime;
	float m_frameTime;
	INT64 m_beginTime, m_endTime;

};