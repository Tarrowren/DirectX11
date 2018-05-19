#pragma once
#pragma comment(lib,"d3d11.lib")//���е�Direct3D����
#pragma comment(lib,"dxgi.lib")//Ӳ���ӿڹ���
#pragma comment(lib,"d3dcompiler.lib")//��ɫ��

#include<d3d11.h>
#include<DirectXMath.h>

using namespace DirectX;

class D3DClass {

public:

	D3DClass();
	~D3DClass();

	bool Initialize(int, int, bool, HWND, bool, float, float);
	void Shutdown();
	void BeginScene(float, float, float, float);
	void EndScene();

	ID3D11Device *GetDevice();
	ID3D11DeviceContext *GetDeviceContext();
	void GetProjectionMatrix(XMMATRIX&);//ͶӰ����
	void GetWorldMatrix(XMMATRIX&);//�������
	void GetOrthoMatrix(XMMATRIX&);//����ͶӰ����
	void GetVideoCardInfo(char*, int&);//������Ϣ

private:

	bool m_vsync_enabled;
	int m_videoCardMemory;
	char m_videoCardDescription[128];
	IDXGISwapChain *m_swapChain;//������
	ID3D11Device *m_device;
	ID3D11DeviceContext *m_deviceContext;
	ID3D11RenderTargetView *m_renderTargetView;//��ȾĿ����ͼ
	ID3D11Texture2D *m_depthStencilBuffer;//���ģ�建��
	ID3D11DepthStencilState *m_depthStencilState;//���ģ��״̬
	ID3D11DepthStencilView *m_depthStencilView;//���ģ����ͼ
	ID3D11RasterizerState *m_rasterState;//��դ״̬
	XMMATRIX m_projectionMatrix;//ͶӰ����
	XMMATRIX m_worldMatrix;//�������
	XMMATRIX m_orthoMatrix;//����ͶӰ����

};