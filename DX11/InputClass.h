#pragma once
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#include<dinput.h>

//#define DIRECTINPUT_VERSION 0x0800

class InputClass {

public:

	InputClass();
	~InputClass();

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();
	bool IsEscapePressed();
	void GetMouseLocation(int&, int&);
	void GetMouseChange(int&, int&);
	void GetMouseWheelChange(int&);

	bool IsLeftPressed();
	bool IsRightPressed();
	bool IsUpPressed();
	bool IsDownPressed();
	bool IsPgUpPressed();
	bool IsPgDownPressed();

	bool IsWPressed();
	bool IsSPressed();
	bool IsAPressed();
	bool IsDPressed();
	bool IsSpacePressed();
	bool IsCPressed();

	bool IsF1Toggled();
	bool IsF2Toggled();

private:

	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();

private:

	IDirectInput8 * m_directInput;
	IDirectInputDevice8* m_keyboard;
	IDirectInputDevice8* m_mouse;

	unsigned char m_keyboardState[256];
	DIMOUSESTATE m_mouseState;

	int m_screenWidth, m_screenHeight;
	int m_mouseX, m_mouseY;
	int m_mouseChangeX, m_mouseChangeY, m_mouseChangeZ;

	bool m_F1_released;
	bool m_F2_released;

};