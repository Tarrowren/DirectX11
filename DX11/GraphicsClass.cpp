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
	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);

	m_Text = new TextClass;
	if (!m_Text) {
		return false;
	}
	result = m_Text->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseViewMatrix);
	if (!result) {
		MessageBox(hwnd, L"�޷���ʼ���ı�����", L"����", MB_OK);

		return false;
	}

	//��������ʼ��ģ�Ͷ���
	m_Model = new ModelClass;
	if (!m_Model) {
		return false;
	}
	result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), (WCHAR*)L"cube.txt", (char*)"stone01.tga");
	if (!result) {
		MessageBox(hwnd, L"�޷���ʼ��ģ�Ͷ���", L"����", MB_OK);
		return false;
	}

	//��������ʼ����ɫ������
	m_LightShader = new LightShaderClass;
	if (!m_LightShader) {
		return false;
	}
	result = m_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, L"�޷���ʼ����ɫ��ɫ������", L"����", MB_OK);
		return false;
	}

	//��������ʼ�������
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

	//����ÿ��֡��
	result = m_Text->SetFps(fps, m_Direct3D->GetDeviceContext());
	if (!result) {
		return false;
	}

	//����CPUʹ����
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

	//����������Կ�ʼ����
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	//���������λ��������ͼ����
	m_Camera->Render();

	//�������D3D�����л�ȡ���硢��ͼ��ͶӰ���ھ���
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	//��ģ�Ͷ��������������������ͼ�ιܵ��ϣ�Ϊ��������׼��
	m_Model->Render(m_Direct3D->GetDeviceContext());

	//ʹ����ɫ��ɫ����Ⱦģ��
	result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(), m_Light->GetDirection(), m_Light->GetDiffuseColor());
	if (!result) {
		return false;
	}

	m_Direct3D->TurnZBufferOff();

	//����Ⱦ�ı�֮ǰ��alpha���
	m_Direct3D->TurnOnAlphaBlending();

	//��Ⱦ�ı��ַ���
	result = m_Text->Render(m_Direct3D->GetDeviceContext(), worldMatrix, orthoMatrix);
	if (!result) {
		return false;
	}

	//����Ⱦ�ı���ر�alpha���
	m_Direct3D->TurnOffAlphaBlending();

	//��������2D��Ⱦ������ɣ������¿���Z������
	m_Direct3D->TurnZBufferOn();

	//����Ⱦ�ĳ������ָ���Ļ
	m_Direct3D->EndScene();

	return true;
}