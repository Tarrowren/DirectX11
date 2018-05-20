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
	IDXGIAdapter *adapter;//适配器
	IDXGIOutput *adapterOutput;//适配器输出
	unsigned int numModes, i, numerator, denominator;
	//unsigned long long stringLength;
	size_t stringLength;
	DXGI_MODE_DESC *displayModeList;//显示模式列表
	DXGI_ADAPTER_DESC adapterDesc;//适配器描述
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;//交换链描述
	D3D_FEATURE_LEVEL featureLevel;//功能级别
	ID3D11Texture2D *backBufferPtr;//后台缓冲区指针
	D3D11_TEXTURE2D_DESC depthBufferDesc;//深度缓冲区描述
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;//深度模板描述
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;//深度模板视图描述
	D3D11_RASTERIZER_DESC rasterDesc;//光栅描述
	D3D11_VIEWPORT viewport;//视口
	float fieldOfView, screenAspect;//视场，屏幕宽高比
	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
	D3D11_BLEND_DESC blendStateDescription;

	m_vsync_enabled = vsync;

	//创建DX图形接口工厂
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result)) {
		return false;
	}

	//用工厂给主图形接口创建适配器（显卡）
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result)) {
		return false;
	}

	//枚举主适配器输出（显示器）
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result)) {
		return false;
	}

	//获取适合适配器输出的DXGI_FORMAT_R8G8B8A8_UNORM显示格式的模式数量
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result)) {
		return false;
	}

	//创建列表来保存显示器/显卡组合的所有可能的显示模式
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList) {
		return false;
	}

	//填充显示模式列表
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result)) {
		return false;
	}

	//遍历所有的显示模式，找到匹配屏幕的宽度和高度，并存储显示器刷新率的分子和分母。
	for (i = 0; i < numModes; i++) {
		if (displayModeList[i].Width == (unsigned int)screenWidth) {
			if (displayModeList[i].Height == (unsigned int)screenHeight) {
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	//获取显卡说明
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result)) {
		return false;
	}

	//显存
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	//将显卡名称储存为字符数组
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0) {
		return false;
	}

	//释放资源
	delete[] displayModeList;
	displayModeList = 0;
	adapterOutput->Release();
	adapterOutput = 0;
	adapter->Release();
	adapter = 0;
	factory->Release();
	factory = 0;

	//初始化交换链描述
	//交换链是指图形绘制的前台和后台缓冲区。通常使用单个后台缓冲区，完成所有绘图后交换到前台缓冲区，然后显示在用户的屏幕上。
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	//设置单个后台缓冲区
	swapChainDesc.BufferCount = 1;

	//设置后台缓冲区的宽高
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	//为后台缓冲区设置常规的32位表面
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	//设置后台缓冲区的刷新率
	if (m_vsync_enabled) {
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else {
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	//设置后台缓冲区的使用情况
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	//设置要渲染的窗口句柄
	swapChainDesc.OutputWindow = hwnd;

	//关闭多重取样
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	if (fullscreen) {
		swapChainDesc.Windowed = false;
	}
	else {
		swapChainDesc.Windowed = true;
	}

	//将扫描线排序和缩放比例设置为未指定
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//显示之后丢弃后台缓冲区内容
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	//不要设置高级标志
	swapChainDesc.Flags = 0;

	//将功能级别设置为DX11
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	//创建交换链，Direct3D设备和设备上下文
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
	if (FAILED(result)) {
		return false;
	}

	//获取指向后台缓冲区的指针
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result)) {
		return false;
	}

	//用指针创建渲染目标视图
	result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
	if (FAILED(result)) {
		return false;
	}

	//释放指针
	backBufferPtr->Release();
	backBufferPtr = 0;

	//初始化深度缓冲区的描述
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	//设置深度缓冲区的描述
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

	//为深度缓冲区创建纹理
	result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if (FAILED(result)) {
		return false;
	}

	//初始化深度模板的描述
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	//设置深度模板的描述,以便控制Direct3D针对每个像素执行哪种类型的深度测试
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	//像素是正面的模板操作
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//像素是背面的模板操作
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//创建深度模板状态
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if (FAILED(result)) {
		return false;
	}

	//设置深度模板状态
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	//初始化深度模板视图
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	//设置深度模板视图描述，让Direct3D知道使用深度缓冲区作为深度模板纹理
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	//创建深度模板视图
	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result)) {
		return false;
	}

	//将渲染目标视图和深度模板缓冲区绑定到输入渲染管道
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	//设置光栅描述，确定怎样绘制多边形以及绘制什么多边形
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

	//创建光栅器状态
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if (FAILED(result)) {
		return false;
	}

	//设置光栅器状态
	m_deviceContext->RSSetState(m_rasterState);

	//设置渲染视点
	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	//创建视点
	m_deviceContext->RSSetViewports(1, &viewport);

	//设置投影矩阵
	fieldOfView = 3.141592654f / 4.0f;
	screenAspect = (float)screenWidth / (float)screenHeight;

	//为3D渲染创建投影矩阵
	m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

	//将世界矩阵初始化为单位矩阵，该矩阵用于将对象的顶点转换为3D场景中的顶点，也用于在3D空间中旋转，平移和缩放对象。
	m_worldMatrix = XMMatrixIdentity();

	//为二维渲染创建正面投影矩阵
	m_orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	//在设置参数之前清除第二个深度模板状态
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

	//清除后台缓冲区
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	//清除深度缓冲区
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

void D3DClass::EndScene() {
	//渲染完成后将后台缓冲区的内容输出到屏幕，考虑刷新率
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