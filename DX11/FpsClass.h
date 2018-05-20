#pragma once
#pragma comment(lib,"winmm.lib")
#include<Windows.h>
#include<mmsystem.h>

class FpsClass {

public:

	FpsClass();
	~FpsClass();

	void Initialize();
	void Frame();
	int GetFps();

private:

	int m_fps, m_count;
	unsigned long m_startTime;

};