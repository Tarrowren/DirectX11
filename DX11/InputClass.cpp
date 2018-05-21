#include"InputClass.h"

InputClass::InputClass() {
	m_directInput = 0;
	m_keyboard = 0;
	m_mouse = 0;
}

InputClass::~InputClass() {

}

bool InputClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight) {
	HRESULT result;

	//存储将用于定位鼠标光标的屏幕大小
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	//在屏幕上初始化鼠标的位置
	m_mouseX = 0;
	m_mouseY = 0;

	m_mouseChangeX = 0;
	m_mouseChangeY = 0;

	//初始化主要的直接输入接口
	result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	if (FAILED(result)) {
		return false;
	}

	//初始化键盘的直接输入接口
	result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	if (FAILED(result)) {
		return false;
	}

	//设置数据格式。在这种情况下，因为它是一个键盘，我们可以使用预定义的数据格式
	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result)) {
		return false;
	}

	//设置键盘的协作级别不与其他程序共享
	result = m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result)) {
		return false;
	}

	//现在获取键盘
	result = m_keyboard->Acquire();
	if (FAILED(result)) {
		return false;
	}

	//初始化鼠标的直接输入界面
	result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	if (FAILED(result)) {
		return false;
	}

	//使用预定义的鼠标数据格式设置鼠标的数据格式
	result = m_mouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result)) {
		return false;
	}

	//设置鼠标的协作级别与其他程序共享
	result = m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result)) {
		return false;
	}

	//获取鼠标
	result = m_mouse->Acquire();
	if (FAILED(result)) {
		return false;
	}

	m_F1_released = true;
	m_F2_released = true;

	return true;
}

void InputClass::Shutdown() {
	if (m_mouse) {
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = 0;
	}
	if (m_keyboard) {
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = 0;
	}
	if (m_directInput) {
		m_directInput->Release();
		m_directInput = 0;
	}

	return;
}

bool InputClass::Frame() {
	bool result;

	//读取键盘的当前状态
	result = ReadKeyboard();
	if (!result) {
		return false;
	}

	//读取鼠标的当前状态
	result = ReadMouse();
	if (!result) {
		return false;
	}

	//处理鼠标和键盘中的更改
	ProcessInput();

	return true;
}

bool InputClass::ReadKeyboard() {
	HRESULT result;

	//读取键盘设备
	result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if (FAILED(result)) {

		//如果键盘失去焦点或没有获得，那么尝试控制回去
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED)) {
			m_keyboard->Acquire();
		}
		else {
			return false;
		}
	}

	return true;
}

bool InputClass::ReadMouse()
{
	HRESULT result;

	//读取鼠标设备。
	result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
	if (FAILED(result)) {
		//如果鼠标失去焦点或没有获取，则尝试重新获得控制权。
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED)) {
			m_mouse->Acquire();
		}
		else {
			return false;
		}
	}

	return true;
}

void InputClass::ProcessInput() {
	//根据帧中鼠标位置的变化更新鼠标光标的位置
	m_mouseX += m_mouseState.lX;
	m_mouseY += m_mouseState.lY;

	//获取鼠标位置变化的值
	m_mouseChangeX = m_mouseState.lX;
	m_mouseChangeY = m_mouseState.lY;

	//确保鼠标位置不超过屏幕宽度或高度。
	if (m_mouseX < 0) {
		m_mouseX = 0;
	}
	if (m_mouseY < 0) {
		m_mouseY = 0;
	}
	if (m_mouseX > m_screenWidth) {
		m_mouseX = m_screenWidth;
	}
	if (m_mouseY > m_screenHeight) {
		m_mouseY = m_screenHeight;
	}

	return;
}

bool InputClass::IsEscapePressed() {
	//在键盘状态下按位进行检查，以检查当前是否正在按下换码键
	if (m_keyboardState[DIK_ESCAPE] & 0x80) {
		return true;
	}

	return false;
}

void InputClass::GetMouseLocation(int& mouseX, int& mouseY) {
	mouseX = m_mouseX;
	mouseY = m_mouseY;

	return;
}

void InputClass::GetMouseChange(int& mouseChangeX, int& mouseChangeY) {
	mouseChangeX = m_mouseChangeX;
	mouseChangeY = m_mouseChangeY;

	return;
}

bool InputClass::IsLeftPressed() {
	if (m_keyboardState[DIK_LEFT] & 0x80) {
		return true;
	}

	return false;
}

bool InputClass::IsRightPressed() {
	if (m_keyboardState[DIK_RIGHT] & 0x80) {
		return true;
	}

	return false;
}

bool InputClass::IsUpPressed() {
	if (m_keyboardState[DIK_UP] & 0x80) {
		return true;
	}

	return false;
}

bool InputClass::IsDownPressed() {
	if (m_keyboardState[DIK_DOWN] & 0x80) {
		return true;
	}

	return false;
}

bool InputClass::IsWPressed() {
	if (m_keyboardState[DIK_W] & 0x80) {
		return true;
	}

	return false;
}

bool InputClass::IsSPressed() {
	if (m_keyboardState[DIK_S] & 0x80) {
		return true;
	}

	return false;
}

bool InputClass::IsAPressed() {
	if (m_keyboardState[DIK_A] & 0x80) {
		return true;
	}

	return false;
}

bool InputClass::IsDPressed() {
	if (m_keyboardState[DIK_D] & 0x80) {
		return true;
	}

	return false;
}

bool InputClass::IsSpacePressed() {
	if (m_keyboardState[DIK_SPACE] & 0x80) {
		return true;
	}

	return false;
}

bool InputClass::IsCPressed() {
	if (m_keyboardState[DIK_C] & 0x80) {
		return true;
	}

	return false;
}

bool InputClass::IsF1Toggled() {
	if (m_keyboardState[DIK_F1] & 0x80) {
		if (m_F1_released) {
			m_F1_released = false;
			return true;
		}
	}
	else {
		m_F1_released = true;
	}

	return false;
}

bool InputClass::IsF2Toggled() {
	if (m_keyboardState[DIK_F2] & 0x80) {
		if (m_F2_released) {
			m_F2_released = false;
			return true;
		}
	}
	else {
		m_F2_released = true;
	}

	return false;
}