#pragma once
#pragma comment(lib,"d3d11.lib")//所有的Direct3D功能
#pragma comment(lib,"dxgi.lib")//硬件接口工具
#pragma comment(lib,"d3dcompiler.lib")//着色器

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
	void GetProjectionMatrix(XMMATRIX&);//投影矩阵
	void GetWorldMatrix(XMMATRIX&);//世界矩阵
	void GetOrthoMatrix(XMMATRIX&);//正面投影矩阵
	void GetVideoCardInfo(char*, int&);//独显信息
	void TurnZBufferOn();
	void TurnZBufferOff();
	void TurnOnCulling();
	void TurnOffCulling();
	void EnableAlphaBlending();
	void EnableAlphaToCoverageBlending();
	void DisableAlphaBlending();
	void EnableWireframe();
	void DisableWireframe();

private:

	bool m_vsync_enabled;
	int m_videoCardMemory;
	char m_videoCardDescription[128];
	IDXGISwapChain *m_swapChain;//交换链
	ID3D11Device *m_device;
	ID3D11DeviceContext *m_deviceContext;
	ID3D11RenderTargetView *m_renderTargetView;//渲染目标视图
	ID3D11Texture2D *m_depthStencilBuffer;//深度模板缓冲
	ID3D11DepthStencilState *m_depthStencilState;//深度模板状态
	ID3D11DepthStencilView *m_depthStencilView;//深度模板视图
	ID3D11RasterizerState *m_rasterState;//光栅状态
	XMMATRIX m_projectionMatrix;//投影矩阵
	XMMATRIX m_worldMatrix;//世界矩阵
	XMMATRIX m_orthoMatrix;//正面投影矩阵
	ID3D11DepthStencilState* m_depthDisabledStencilState;
	ID3D11BlendState* m_alphaEnableBlendingState;
	ID3D11BlendState* m_alphaEnableBlendingState2;
	ID3D11BlendState* m_alphaDisableBlendingState;
	ID3D11RasterizerState* m_rasterStateNoCulling;
	ID3D11RasterizerState* m_rasterStateWireframe;

};