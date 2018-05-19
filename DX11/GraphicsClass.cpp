#include"GraphicsClass.h"

GraphicsClass::GraphicsClass() {
	m_Direct3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_ColorShader = 0;
}

GraphicsClass::~GraphicsClass() {

}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd) {
	bool result;

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

	//创建并初始化模型对象
	m_Model = new ModelClass;
	if (!m_Model) {
		return false;
	}
	result = m_Model->Initialize(m_Direct3D->GetDevice(), (WCHAR*)L"cube.txt");
	if (!result) {
		MessageBox(hwnd, L"无法初始化模型对象。", L"错误", MB_OK);
		return false;
	}

	//创建并初始化颜色着色器对象
	m_ColorShader = new ColorShaderClass;
	if (!m_ColorShader) {
		return false;
	}
	result = m_ColorShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, L"无法初始化颜色着色器对象。", L"错误", MB_OK);
		return false;
	}

	return true;
}

void GraphicsClass::Shutdown() {
	if (m_ColorShader) {
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
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

bool GraphicsClass::Frame() {
	bool result;

	//渲染图形场景
	result = Render();
	if (!result) {
		return false;
	}

	return true;
}

bool GraphicsClass::Render() {
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	//清除缓冲区以开始场景
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	//根据相机的位置生成视图矩阵
	m_Camera->Render();

	//从相机和D3D对象中获取世界、视图和投影矩阵
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	//将模型顶点和索引缓冲区放置在图形管道上，为绘制做好准备
	m_Model->Render(m_Direct3D->GetDeviceContext());

	//使用颜色着色器渲染模型
	result = m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	if (!result) {
		return false;
	}

	//将渲染的场景呈现给屏幕
	m_Direct3D->EndScene();

	return true;
}