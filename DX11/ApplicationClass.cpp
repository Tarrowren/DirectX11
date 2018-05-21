#include"ApplicationClass.h"

ApplicationClass::ApplicationClass() {
	m_Input = 0;
	m_Direct3D = 0;
	m_Timer = 0;
	m_Fps = 0;
	m_ShaderManager = 0;
	m_Zone = 0;
}

ApplicationClass::~ApplicationClass() {

}

bool ApplicationClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight) {
	bool result;

	m_Input = new InputClass;
	if (!m_Input) {
		return false;
	}
	result = m_Input->Initialize(hinstance, hwnd, screenWidth, screenHeight);
	if (!result) {
		MessageBox(hwnd, L"�޷���ʼ���������", L"����", MB_OK);
		return false;
	}

	m_Direct3D = new D3DClass;
	if (!m_Direct3D) {
		return false;
	}
	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result) {
		MessageBox(hwnd, L"�޷���ʼ��Direct3D����", L"����", MB_OK);
		return false;
	}

	m_ShaderManager = new ShaderManagerClass;
	if (!m_ShaderManager) {
		return false;
	}
	result = m_ShaderManager->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, L"�޷���ʼ����ɫ����������", L"����", MB_OK);
		return false;
	}

	m_Timer = new TimerClass;
	if (!m_Timer) {
		return false;
	}
	result = m_Timer->Initialize();
	if (!result) {
		MessageBox(hwnd, L"�޷���ʼ����ʱ������", L"����", MB_OK);
		return false;
	}

	m_Fps = new FpsClass;
	if (!m_Fps) {
		return false;
	}
	m_Fps->Initialize();

	m_Zone = new ZoneClass;
	if (!m_Zone) {
		return false;
	}
	result = m_Zone->Initialize(m_Direct3D, hwnd, screenWidth, screenHeight, SCREEN_DEPTH);
	if (!result) {
		MessageBox(hwnd, L"�޷���ʼ���ռ����", L"����", MB_OK);
		return false;
	}

	return true;
}

void ApplicationClass::Shutdown() {
	if (m_Zone) {
		m_Zone->Shutdown();
		delete m_Zone;
		m_Zone = 0;
	}
	if (m_Fps) {
		delete m_Fps;
		m_Fps = 0;
	}
	if (m_Timer) {
		delete m_Timer;
		m_Timer = 0;
	}
	if (m_ShaderManager) {
		m_ShaderManager->Shutdown();
		delete m_ShaderManager;
		m_ShaderManager = 0;
	}
	if (m_Direct3D) {
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}
	if (m_Input) {
		m_Input->Shutdown();
		delete m_Input;
		m_Input = 0;
	}

	return;
}

bool ApplicationClass::Frame() {
	bool result;

	m_Fps->Frame();
	m_Timer->Frame();

	result = m_Input->Frame();
	if (!result) {
		return false;
	}

	if (m_Input->IsEscapePressed() == true) {
		return false;
	}

	result = m_Zone->Frame(m_Direct3D, m_Input, m_ShaderManager, m_Timer->GetTime(), m_Fps->GetFps());
	if (!result) {
		return false;
	}

	return result;
}