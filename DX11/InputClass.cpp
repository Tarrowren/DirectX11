#include"InputClass.h"

InputClass::InputClass() {

}

InputClass::~InputClass() {

}

void InputClass::Initialize() {
	//初始化所有按键的状态
	for (int i = 0; i < 256; i++) {
		m_keys[i] = false;
	}
	return;
}

//记录被按下的键
void InputClass::KeyDown(unsigned int key) {
	m_keys[key] = true;
	return;
}

//记录被松开的键
void InputClass::KeyUp(unsigned int key) {
	m_keys[key] = false;
	return;
}

bool InputClass::IsKeyDown(unsigned int key) {
	return m_keys[key];
}