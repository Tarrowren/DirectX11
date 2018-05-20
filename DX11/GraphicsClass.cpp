#include"GraphicsClass.h"

GraphicsClass::GraphicsClass() {
	m_Direct3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_LightShader = 0;
	m_Light = 0;
	m_Text = 0;
}

GraphicsClass::~GraphicsClass() {

}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd) {
	bool result;
	XMMATRIX baseViewMatrix;

	//创建并初始化Direct3D对象
	m_Direct3D = new D3DClass;
	if (!m_Direct3D) {
		return false;
	}
	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result) {
		MessageBox(hwnd, L"无法初始化Direct3D。", L"错误", MB_OK);
		return false;
	}

	//创建相机对象，设置相机的初始位置
	m_Camera = new CameraClass;
	if (!m_Camera) {
		return false;
	}
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);
	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);

	m_Text = new TextClass;
	if (!m_Text) {
		return false;
	}
	result = m_Text->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseViewMatrix);
	if (!result) {
		MessageBox(hwnd, L"无法初始化文本对象。", L"错误", MB_OK);

		return false;
	}

	//创建并初始化模型对象
	m_Model = new ModelClass;
	if (!m_Model) {
		return false;
	}
	result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), (WCHAR*)L"cube.txt", (char*)"stone01.tga");
	if (!result) {
		MessageBox(hwnd, L"无法初始化模型对象。", L"错误", MB_OK);
		return false;
	}

	//创建并初始化着色器对象
	m_LightShader = new LightShaderClass;
	if (!m_LightShader) {
		return false;
	}
	result = m_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, L"无法初始化颜色着色器对象。", L"错误", MB_OK);
		return false;
	}

	//创建并初始化光对象
	m_Light = new LightClass();
	if (!m_Light) {
		return false;
	}
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);

	return true;
}

void GraphicsClass::Shutdown() {
	if (m_Text) {
		m_Text->Shutdown();
		delete m_Text;
		m_Text = 0;
	}
	if (m_Light) {
		delete m_Light;
		m_Light = 0;
	}
	if (m_LightShader) {
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}
	if (m_Model) {
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}
	if (m_Camera) {
		delete m_Camera;
		m_Camera = 0;
	}
	if (m_Direct3D) {
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}

	return;
}

bool GraphicsClass::Frame(int fps, int xcpu, float frameTime) {
	bool result;

	//设置每秒帧数
	result = m_Text->SetFps(fps, m_Direct3D->GetDeviceContext());
	if (!result) {
		return false;
	}

	//设置CPU使用率
	result = m_Text->SetCpu(xcpu, m_Direct3D->GetDeviceContext());
	if (!result) {
		return false;
	}

	return true;
}

bool GraphicsClass::Render() {
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	bool result;
	static float rotation = 0.0f;

	//清除缓冲区以开始场景
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	//根据相机的位置生成视图矩阵
	m_Camera->Render();

	//从相机和D3D对象中获取世界、视图、投影和邻矩阵
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	//将模型顶点和索引缓冲区放置在图形管道上，为绘制做好准备
	m_Model->Render(m_Direct3D->GetDeviceContext());

	//使用颜色着色器渲染模型
	result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(), m_Light->GetDirection(), m_Light->GetDiffuseColor());
	if (!result) {
		return false;
	}

	m_Direct3D->TurnZBufferOff();

	//在渲染文本之前打开alpha混合
	m_Direct3D->TurnOnAlphaBlending();

	//渲染文本字符串
	result = m_Text->Render(m_Direct3D->GetDeviceContext(), worldMatrix, orthoMatrix);
	if (!result) {
		return false;
	}

	//在渲染文本后关闭alpha混合
	m_Direct3D->TurnOffAlphaBlending();

	//现在所有2D渲染都已完成，请重新开启Z缓冲区
	m_Direct3D->TurnZBufferOn();

	//将渲染的场景呈现给屏幕
	m_Direct3D->EndScene();

	return true;
}