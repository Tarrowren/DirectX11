#include"InputClass.h"

InputClass::InputClass() {

}

InputClass::~InputClass() {

}

void InputClass::Initialize() {
	//��ʼ�����а�����״̬
	for (int i = 0; i < 256; i++) {
		m_keys[i] = false;
	}
	return;
}

//��¼�����µļ�
void InputClass::KeyDown(unsigned int key) {
	m_keys[key] = true;
	return;
}

//��¼���ɿ��ļ�
void InputClass::KeyUp(unsigned int key) {
	m_keys[key] = false;
	return;
}

bool InputClass::IsKeyDown(unsigned int key) {
	return m_keys[key];
}