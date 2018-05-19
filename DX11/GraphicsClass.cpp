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

	//��������ʼ��Direct3D����
	m_Direct3D = new D3DClass;
	if (!m_Direct3D) {
		return false;
	}
	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result) {
		MessageBox(hwnd, L"�޷���ʼ��Direct3D��", L"����", MB_OK);
		return false;
	}

	//�������������������ĳ�ʼλ��
	m_Camera = new CameraClass;
	if (!m_Camera) {
		return false;
	}
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);

	//��������ʼ��ģ�Ͷ���
	m_Model = new ModelClass;
	if (!m_Model) {
		return false;
	}
	result = m_Model->Initialize(m_Direct3D->GetDevice(), (WCHAR*)L"cube.txt");
	if (!result) {
		MessageBox(hwnd, L"�޷���ʼ��ģ�Ͷ���", L"����", MB_OK);
		return false;
	}

	//��������ʼ����ɫ��ɫ������
	m_ColorShader = new ColorShaderClass;
	if (!m_ColorShader) {
		return false;
	}
	result = m_ColorShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, L"�޷���ʼ����ɫ��ɫ������", L"����", MB_OK);
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

	//��Ⱦͼ�γ���
	result = Render();
	if (!result) {
		return false;
	}

	return true;
}

bool GraphicsClass::Render() {
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	//����������Կ�ʼ����
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	//���������λ��������ͼ����
	m_Camera->Render();

	//�������D3D�����л�ȡ���硢��ͼ��ͶӰ����
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	//��ģ�Ͷ��������������������ͼ�ιܵ��ϣ�Ϊ��������׼��
	m_Model->Render(m_Direct3D->GetDeviceContext());

	//ʹ����ɫ��ɫ����Ⱦģ��
	result = m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	if (!result) {
		return false;
	}

	//����Ⱦ�ĳ������ָ���Ļ
	m_Direct3D->EndScene();

	return true;
}