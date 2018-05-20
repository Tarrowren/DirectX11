#pragma once
#include<Windows.h>

#include"InputClass.h"
#include"GraphicsClass.h"
#include"FpsClass.h"
#include"CpuClass.h"
#include"TimerClass.h"

class SystemClass {

public:

	SystemClass();
	~SystemClass();

	bool Initialize();
	void Shutdown();
	void Run();
	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:

	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

private:

	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	InputClass* m_Input;
	GraphicsClass* m_Graphics;
	FpsClass* m_Fps;
	CpuClass* m_Cpu;
	TimerClass* m_Timer;

};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static SystemClass *ApplicationHandle = 0;