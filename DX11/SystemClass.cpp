#include"SystemClass.h"

SystemClass::SystemClass() {
	m_Input = 0;
	m_Graphics = 0;
	m_Fps = 0;
	m_Cpu = 0;
	m_Timer = 0;
}

SystemClass::~SystemClass() {

}

bool SystemClass::Initialize() {
	int screenWidth, screenHeight;
	bool result;

	screenWidth = 0;
	screenHeight = 0;

	//��ʼ��Windows API
	InitializeWindows(screenWidth, screenHeight);

	//��������ʼ���������
	m_Input = new InputClass;
	if (!m_Input) {
		return false;
	}
	m_Input->Initialize();

	//��������ʼ��ͼ�ζ���
	m_Graphics = new GraphicsClass;
	if (!m_Graphics) {
		return false;
	}
	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
	if (!result) {
		return false;
	}

	//��������ʼ��Fps����
	m_Fps = new FpsClass;
	if (!m_Fps) {
		return false;
	}
	m_Fps->Initialize();

	//��������ʼ��Cpu����
	m_Cpu = new CpuClass;
	if (!m_Cpu) {
		return false;
	}
	m_Cpu->Initialize();

	//��������ʼ��Timer����
	m_Timer = new TimerClass;
	if (!m_Timer) {
		return false;
	}
	result = m_Timer->Initialize();
	if (!result) {
		MessageBox(m_hwnd, L"�޷���ʼ��Timer����", L"����", MB_OK);
		return false;
	}

	return true;
}

void SystemClass::Shutdown() {
	if (m_Timer) {
		delete m_Timer;
		m_Timer = 0;
	}
	if (m_Cpu) {
		m_Cpu->Shutdown();
		delete m_Cpu;
		m_Cpu = 0;
	}
	if (m_Fps) {
		delete m_Fps;
		m_Fps = 0;
	}
	if (m_Input) {
		delete m_Input;
		m_Input = 0;
	}
	if (m_Graphics) {
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}
	ShutdownWindows();

	return;
}

void SystemClass::Run() {
	MSG msg;
	bool done = false, result;

	//��ʼ����Ϣ�ṹ
	ZeroMemory(&msg, sizeof(MSG));

	while (!done) {
		//��������Ϣ
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//�ر�Ӧ��֮���˳�
		if (msg.message == WM_QUIT) {
			done = true;
		}
		else {
			//֡����
			result = Frame();
			if (!result) {
				done = true;
			}
		}
	}

	return;
}

bool SystemClass::Frame() {
	bool result;

	m_Timer->Frame();
	m_Fps->Frame();
	m_Cpu->Frame();

	//�����˳���
	if (m_Input->IsKeyDown(VK_ESCAPE)) {
		return false;
	}

	//��ͼ�ζ���ִ��֡����
	result = m_Graphics->Frame(m_Fps->GetFps(), m_Cpu->GetCpuPercentage(), m_Timer->GetTime());
	if (!result) {
		return false;
	}

	result = m_Graphics->Render();
	if (!result) {
		return false;
	}

	return true;
}

//�Զ�����Ϣ������
LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam) {
	switch (umsg) {
		//����Ƿ��м�������
	case WM_KEYDOWN: {
		m_Input->KeyDown((unsigned int)wparam);
		return 0;
	}
					 //����Ƿ��м����ɿ�
	case WM_KEYUP: {
		m_Input->KeyUp((unsigned int)wparam);
		return 0;
	}
				   //������Ϣ���͵�Ĭ����Ϣ������
	default: {
		return DefWindowProc(hwnd, umsg, wparam, lparam);
	}
	}
}

void SystemClass::InitializeWindows(int &screenWidth, int &screenHeight) {
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	//��ȡָ���������ⲿָ��
	ApplicationHandle = this;

	//��ȡ��Ӧ�õ�ʵ��
	m_hinstance = GetModuleHandle(NULL);

	//Ӧ�ó�������
	m_applicationName = L"����";

	//�������Ĭ������
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	//ע�ᴰ����
	RegisterClassEx(&wc);

	//ȷ��������Ļ�ֱ���
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	//��Ļ����
	if (FULL_SCREEN) {
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned int)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned int)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		posX = posY = 0;
	}
	else {
		screenWidth = 800;
		screenHeight = 600;

		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	//����Ļ���ô������ڲ���ȡ���
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP, posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	//��ʾ���ڲ���ȡ����
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	//�������ָ��
	ShowCursor(false);

	return;
}

void SystemClass::ShutdownWindows() {
	//��ʾ���ָ��
	ShowCursor(true);

	//ȫ��״̬�˳�ȫ��
	if (FULL_SCREEN) {
		ChangeDisplaySettings(NULL, 0);
	}

	//ɾ������
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	//ɾ��Ӧ��ʵ��
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	//�ͷ�ָ���������ⲿָ��
	ApplicationHandle = NULL;

	return;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam) {
	switch (umessage) {
		//��鴰���Ƿ�����
	case WM_DESTROY: {
		PostQuitMessage(0);
		return 0;
	}
					 //��鴰���Ƿ񱻹ر�
	case WM_CLOSE: {
		PostQuitMessage(0);
		return 0;
	}
				   //������Ϣ���͵�Ĭ����Ϣ������
	default: {
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}