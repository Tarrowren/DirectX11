#pragma once

#include"D3DClass.h"
#include"InputClass.h"
#include"TimerClass.h"
#include"FpsClass.h"
#include"CpuClass.h"
#include"ShaderManagerClass.h"
#include"ZoneClass.h"

const bool FULL_SCREEN = true;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class ApplicationClass {

public:

	ApplicationClass();
	~ApplicationClass();

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();

private:

	InputClass * m_Input;
	D3DClass* m_Direct3D;
	ShaderManagerClass* m_ShaderManager;
	TimerClass* m_Timer;
	FpsClass* m_Fps;
	CpuClass* m_Cpu;
	ZoneClass* m_Zone;

};