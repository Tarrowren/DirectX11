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

	//�洢�����ڶ�λ��������Ļ��С
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	//����Ļ�ϳ�ʼ������λ��
	m_mouseX = 0;
	m_mouseY = 0;

	m_mouseChangeX = 0;
	m_mouseChangeY = 0;

	//��ʼ����Ҫ��ֱ������ӿ�
	result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	if (FAILED(result)) {
		return false;
	}

	//��ʼ�����̵�ֱ������ӿ�
	result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	if (FAILED(result)) {
		return false;
	}

	//�������ݸ�ʽ������������£���Ϊ����һ�����̣����ǿ���ʹ��Ԥ��������ݸ�ʽ
	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result)) {
		return false;
	}

	//���ü��̵�Э������������������
	result = m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result)) {
		return false;
	}

	//���ڻ�ȡ����
	result = m_keyboard->Acquire();
	if (FAILED(result)) {
		return false;
	}

	//��ʼ������ֱ���������
	result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	if (FAILED(result)) {
		return false;
	}

	//ʹ��Ԥ�����������ݸ�ʽ�����������ݸ�ʽ
	result = m_mouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result)) {
		return false;
	}

	//��������Э������������������
	result = m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result)) {
		return false;
	}

	//��ȡ���
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

	//��ȡ���̵ĵ�ǰ״̬
	result = ReadKeyboard();
	if (!result) {
		return false;
	}

	//��ȡ���ĵ�ǰ״̬
	result = ReadMouse();
	if (!result) {
		return false;
	}

	//�������ͼ����еĸ���
	ProcessInput();

	return true;
}

bool InputClass::ReadKeyboard() {
	HRESULT result;

	//��ȡ�����豸
	result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if (FAILED(result)) {

		//�������ʧȥ�����û�л�ã���ô���Կ��ƻ�ȥ
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

	//��ȡ����豸��
	result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
	if (FAILED(result)) {
		//������ʧȥ�����û�л�ȡ���������»�ÿ���Ȩ��
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
	//����֡�����λ�õı仯����������λ��
	m_mouseX += m_mouseState.lX;
	m_mouseY += m_mouseState.lY;

	//��ȡ���λ�ñ仯��ֵ
	m_mouseChangeX = m_mouseState.lX;
	m_mouseChangeY = m_mouseState.lY;

	//ȷ�����λ�ò�������Ļ��Ȼ�߶ȡ�
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
	//�ڼ���״̬�°�λ���м�飬�Լ�鵱ǰ�Ƿ����ڰ��»����
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