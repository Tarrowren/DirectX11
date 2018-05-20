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

	//初始化Windows API
	InitializeWindows(screenWidth, screenHeight);

	//创建并初始化输入对象
	m_Input = new InputClass;
	if (!m_Input) {
		return false;
	}
	m_Input->Initialize();

	//创建并初始化图形对象
	m_Graphics = new GraphicsClass;
	if (!m_Graphics) {
		return false;
	}
	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
	if (!result) {
		return false;
	}

	//创建并初始化Fps对象
	m_Fps = new FpsClass;
	if (!m_Fps) {
		return false;
	}
	m_Fps->Initialize();

	//创建并初始化Cpu对象
	m_Cpu = new CpuClass;
	if (!m_Cpu) {
		return false;
	}
	m_Cpu->Initialize();

	//创建并初始化Timer对象
	m_Timer = new TimerClass;
	if (!m_Timer) {
		return false;
	}
	result = m_Timer->Initialize();
	if (!result) {
		MessageBox(m_hwnd, L"无法初始化Timer对象", L"错误", MB_OK);
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

	//初始化消息结构
	ZeroMemory(&msg, sizeof(MSG));

	while (!done) {
		//处理窗口消息
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//关闭应用之后退出
		if (msg.message == WM_QUIT) {
			done = true;
		}
		else {
			//帧处理
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

	//按下退出键
	if (m_Input->IsKeyDown(VK_ESCAPE)) {
		return false;
	}

	//对图形对象执行帧处理
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

//自定义信息处理器
LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam) {
	switch (umsg) {
		//检查是否有键被按下
	case WM_KEYDOWN: {
		m_Input->KeyDown((unsigned int)wparam);
		return 0;
	}
					 //检查是否有键被松开
	case WM_KEYUP: {
		m_Input->KeyUp((unsigned int)wparam);
		return 0;
	}
				   //其他消息发送到默认信息处理器
	default: {
		return DefWindowProc(hwnd, umsg, wparam, lparam);
	}
	}
}

void SystemClass::InitializeWindows(int &screenWidth, int &screenHeight) {
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	//获取指向这个类的外部指针
	ApplicationHandle = this;

	//获取该应用的实例
	m_hinstance = GetModuleHandle(NULL);

	//应用程序名称
	m_applicationName = L"引擎";

	//窗口类的默认设置
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

	//注册窗口类
	RegisterClassEx(&wc);

	//确定桌面屏幕分辨率
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	//屏幕设置
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

	//用屏幕设置创建窗口并获取句柄
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP, posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	//显示窗口并获取焦点
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	//隐藏鼠标指针
	ShowCursor(false);

	return;
}

void SystemClass::ShutdownWindows() {
	//显示鼠标指针
	ShowCursor(true);

	//全屏状态退出全屏
	if (FULL_SCREEN) {
		ChangeDisplaySettings(NULL, 0);
	}

	//删除窗口
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	//删除应用实例
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	//释放指向这个类的外部指针
	ApplicationHandle = NULL;

	return;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam) {
	switch (umessage) {
		//检查窗口是否被销毁
	case WM_DESTROY: {
		PostQuitMessage(0);
		return 0;
	}
					 //检查窗口是否被关闭
	case WM_CLOSE: {
		PostQuitMessage(0);
		return 0;
	}
				   //其他消息发送到默认信息处理器
	default: {
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}