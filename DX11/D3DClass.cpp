#include"D3DClass.h"

D3DClass::D3DClass() {
	m_swapChain = 0;
	m_device = 0;
	m_deviceContext = 0;
	m_renderTargetView = 0;
	m_depthStencilBuffer = 0;
	m_depthStencilState = 0;
	m_depthStencilView = 0;
	m_rasterState = 0;
	m_depthDisabledStencilState = 0;
	m_alphaEnableBlendingState = 0;
	m_alphaDisableBlendingState = 0;
}

D3DClass::~D3DClass() {

}

bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear) {
	HRESULT result;
	IDXGIFactory *factory;
	IDXGIAdapter *adapter;//������
	IDXGIOutput *adapterOutput;//���������
	unsigned int numModes, i, numerator, denominator;
	//unsigned long long stringLength;
	size_t stringLength;
	DXGI_MODE_DESC *displayModeList;//��ʾģʽ�б�
	DXGI_ADAPTER_DESC adapterDesc;//����������
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;//����������
	D3D_FEATURE_LEVEL featureLevel;//���ܼ���
	ID3D11Texture2D *backBufferPtr;//��̨������ָ��
	D3D11_TEXTURE2D_DESC depthBufferDesc;//��Ȼ���������
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;//���ģ������
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;//���ģ����ͼ����
	D3D11_RASTERIZER_DESC rasterDesc;//��դ����
	D3D11_VIEWPORT viewport;//�ӿ�
	float fieldOfView, screenAspect;//�ӳ�����Ļ��߱�
	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
	D3D11_BLEND_DESC blendStateDescription;

	m_vsync_enabled = vsync;

	//����DXͼ�νӿڹ���
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result)) {
		return false;
	}

	//�ù�������ͼ�νӿڴ������������Կ���
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result)) {
		return false;
	}

	//ö�����������������ʾ����
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result)) {
		return false;
	}

	//��ȡ�ʺ������������DXGI_FORMAT_R8G8B8A8_UNORM��ʾ��ʽ��ģʽ����
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result)) {
		return false;
	}

	//�����б���������ʾ��/�Կ���ϵ����п��ܵ���ʾģʽ
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList) {
		return false;
	}

	//�����ʾģʽ�б�
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result)) {
		return false;
	}

	//�������е���ʾģʽ���ҵ�ƥ����Ļ�Ŀ�Ⱥ͸߶ȣ����洢��ʾ��ˢ���ʵķ��Ӻͷ�ĸ��
	for (i = 0; i < numModes; i++) {
		if (displayModeList[i].Width == (unsigned int)screenWidth) {
			if (displayModeList[i].Height == (unsigned int)screenHeight) {
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	//��ȡ�Կ�˵��
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result)) {
		return false;
	}

	//�Դ�
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	//���Կ����ƴ���Ϊ�ַ�����
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0) {
		return false;
	}

	//�ͷ���Դ
	delete[] displayModeList;
	displayModeList = 0;
	adapterOutput->Release();
	adapterOutput = 0;
	adapter->Release();
	adapter = 0;
	factory->Release();
	factory = 0;

	//��ʼ������������
	//��������ָͼ�λ��Ƶ�ǰ̨�ͺ�̨��������ͨ��ʹ�õ�����̨��������������л�ͼ�󽻻���ǰ̨��������Ȼ����ʾ���û�����Ļ�ϡ�
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	//���õ�����̨������
	swapChainDesc.BufferCount = 1;

	//���ú�̨�������Ŀ��
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	//Ϊ��̨���������ó����32λ����
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	//���ú�̨��������ˢ����
	if (m_vsync_enabled) {
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else {
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	//���ú�̨��������ʹ�����
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	//����Ҫ��Ⱦ�Ĵ��ھ��
	swapChainDesc.OutputWindow = hwnd;

	//�رն���ȡ��
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	if (fullscreen) {
		swapChainDesc.Windowed = false;
	}
	else {
		swapChainDesc.Windowed = true;
	}

	//��ɨ������������ű�������Ϊδָ��
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//��ʾ֮������̨����������
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	//��Ҫ���ø߼���־
	swapChainDesc.Flags = 0;

	//�����ܼ�������ΪDX11
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	//������������Direct3D�豸���豸������
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
	if (FAILED(result)) {
		return false;
	}

	//��ȡָ���̨��������ָ��
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result)) {
		return false;
	}

	//��ָ�봴����ȾĿ����ͼ
	result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
	if (FAILED(result)) {
		return false;
	}

	//�ͷ�ָ��
	backBufferPtr->Release();
	backBufferPtr = 0;

	//��ʼ����Ȼ�����������
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	//������Ȼ�����������
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	//Ϊ��Ȼ�������������
	result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if (FAILED(result)) {
		return false;
	}

	//��ʼ�����ģ�������
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	//�������ģ�������,�Ա����Direct3D���ÿ������ִ���������͵���Ȳ���
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	//�����������ģ�����
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//�����Ǳ����ģ�����
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//�������ģ��״̬
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if (FAILED(result)) {
		return false;
	}

	//�������ģ��״̬
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	//��ʼ�����ģ����ͼ
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	//�������ģ����ͼ��������Direct3D֪��ʹ����Ȼ�������Ϊ���ģ������
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	//�������ģ����ͼ
	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result)) {
		return false;
	}

	//����ȾĿ����ͼ�����ģ�建�����󶨵�������Ⱦ�ܵ�
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	//���ù�դ������ȷ���������ƶ�����Լ�����ʲô�����
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	//������դ��״̬
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if (FAILED(result)) {
		return false;
	}

	//���ù�դ��״̬
	m_deviceContext->RSSetState(m_rasterState);

	//������Ⱦ�ӵ�
	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	//�����ӵ�
	m_deviceContext->RSSetViewports(1, &viewport);

	//����ͶӰ����
	fieldOfView = 3.141592654f / 4.0f;
	screenAspect = (float)screenWidth / (float)screenHeight;

	//Ϊ3D��Ⱦ����ͶӰ����
	m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

	//����������ʼ��Ϊ��λ���󣬸þ������ڽ�����Ķ���ת��Ϊ3D�����еĶ��㣬Ҳ������3D�ռ�����ת��ƽ�ƺ����Ŷ���
	m_worldMatrix = XMMatrixIdentity();

	//Ϊ��ά��Ⱦ��������ͶӰ����
	m_orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	//�����ò���֮ǰ����ڶ������ģ��״̬
	ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));

	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDisabledStencilDesc.StencilEnable = true;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;
	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	result = m_device->CreateDepthStencilState(&depthDisabledStencilDesc, &m_depthDisabledStencilState);
	if (FAILED(result)) {
		return false;
	}

	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));

	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	result = m_device->CreateBlendState(&blendStateDescription, &m_alphaEnableBlendingState);
	if (FAILED(result)) {
		return false;
	}

	blendStateDescription.RenderTarget[0].BlendEnable = FALSE;

	result = m_device->CreateBlendState(&blendStateDescription, &m_alphaDisableBlendingState);
	if (FAILED(result)) {
		return false;
	}

	return true;
}

void D3DClass::Shutdown() {
	if (m_swapChain) {
		m_swapChain->SetFullscreenState(false, NULL);
	}
	if (m_alphaEnableBlendingState) {
		m_alphaEnableBlendingState->Release();
		m_alphaEnableBlendingState = 0;
	}
	if (m_alphaDisableBlendingState) {
		m_alphaDisableBlendingState->Release();
		m_alphaDisableBlendingState = 0;
	}
	if (m_rasterState) {
		m_rasterState->Release();
		m_rasterState = 0;
	}
	if (m_depthStencilView) {
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}
	if (m_depthDisabledStencilState) {
		m_depthDisabledStencilState->Release();
		m_depthDisabledStencilState = 0;
	}
	if (m_depthStencilState) {
		m_depthStencilState->Release();
		m_depthStencilState = 0;
	}

	if (m_depthStencilBuffer) {
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}

	if (m_renderTargetView) {
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}

	if (m_deviceContext) {
		m_deviceContext->Release();
		m_deviceContext = 0;
	}

	if (m_device) {
		m_device->Release();
		m_device = 0;
	}

	if (m_swapChain) {
		m_swapChain->Release();
		m_swapChain = 0;
	}

	return;
}

void D3DClass::TurnOnAlphaBlending() {
	float blendFactor[4];

	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;

	m_deviceContext->OMSetBlendState(m_alphaEnableBlendingState, blendFactor, 0xffffffff);

	return;
}

void D3DClass::TurnOffAlphaBlending() {
	float blendFactor[4];

	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;

	m_deviceContext->OMSetBlendState(m_alphaDisableBlendingState, blendFactor, 0xffffffff);

	return;
}


void D3DClass::BeginScene(float red, float green, float blue, float alpha) {
	float color[4];

	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	//�����̨������
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	//�����Ȼ�����
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

void D3DClass::EndScene() {
	//��Ⱦ��ɺ󽫺�̨�������������������Ļ������ˢ����
	if (m_vsync_enabled) {
		m_swapChain->Present(1, 0);
	}
	else {
		m_swapChain->Present(0, 0);
	}

	return;
}

ID3D11Device *D3DClass::GetDevice() {
	return m_device;
}

ID3D11DeviceContext *D3DClass::GetDeviceContext() {
	return m_deviceContext;
}

void D3DClass::GetProjectionMatrix(XMMATRIX &projectionMatrix) {
	projectionMatrix = m_projectionMatrix;
	return;
}

void D3DClass::GetWorldMatrix(XMMATRIX &worldMatrix) {
	worldMatrix = m_worldMatrix;
	return;
}

void D3DClass::GetOrthoMatrix(XMMATRIX &orthoMatrix) {
	orthoMatrix = m_orthoMatrix;
	return;
}

void D3DClass::GetVideoCardInfo(char *cardName, int &memory) {
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;

	return;
}

void D3DClass::TurnZBufferOn() {
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	return;
}

void D3DClass::TurnZBufferOff() {
	m_deviceContext->OMSetDepthStencilState(m_depthDisabledStencilState, 1);

	return;
}