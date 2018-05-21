#include"FontShaderClass.h"

FontShaderClass::FontShaderClass() {
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_constantBuffer = 0;
	m_samplerState = 0;
	m_pixelBuffer = 0;
}

FontShaderClass::~FontShaderClass() {

}

bool FontShaderClass::Initialize(ID3D11Device* device, HWND hwnd) {
	bool result;

	result = InitializeShader(device, hwnd, (WCHAR*)L"FontVertexShader.hlsl", (WCHAR*)L"FontPixelShader.hlsl");
	if (!result) {
		return false;
	}

	return true;
}

void FontShaderClass::Shutdown() {
	ShutdownShader();

	return;
}

bool FontShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT4 pixelColor) {
	bool result;

	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, pixelColor);
	if (!result) {
		return false;
	}
	RenderShader(deviceContext, indexCount);

	return true;
}

bool FontShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename) {
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC constantBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC pixelBufferDesc;

	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	result = D3DCompileFromFile(vsFilename, NULL, NULL, "FontVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result)) {
		if (errorMessage) {
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		else {
			MessageBox(hwnd, vsFilename, L"缺少顶点着色器文件", MB_OK);
		}

		return false;
	}

	result = D3DCompileFromFile(psFilename, NULL, NULL, "FontPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result)) {
		if (errorMessage) {
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		else {
			MessageBox(hwnd, psFilename, L"缺少像素着色器文件", MB_OK);
		}

		return false;
	}

	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result)) {
		return false;
	}

	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result)) {
		return false;
	}

	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;
	
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result)) {
		return false;
	}

	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantBufferDesc.ByteWidth = sizeof(ConstantBufferType);
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantBufferDesc.MiscFlags = 0;
	constantBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&constantBufferDesc, NULL, &m_constantBuffer);
	if (FAILED(result)) {
		return false;
	}

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	result = device->CreateSamplerState(&samplerDesc, &m_samplerState);
	if (FAILED(result)) {
		return false;
	}

	pixelBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	pixelBufferDesc.ByteWidth = sizeof(PixelBufferType);
	pixelBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	pixelBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pixelBufferDesc.MiscFlags = 0;
	pixelBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&pixelBufferDesc, NULL, &m_pixelBuffer);
	if (FAILED(result)) {
		return false;
	}

	return true;
}

void FontShaderClass::ShutdownShader() {
	if (m_pixelBuffer) {
		m_pixelBuffer->Release();
		m_pixelBuffer = 0;
	}
	if (m_samplerState) {
		m_samplerState->Release();
		m_samplerState = 0;
	}
	if (m_constantBuffer) {
		m_constantBuffer->Release();
		m_constantBuffer = 0;
	}
	if (m_layout) {
		m_layout->Release();
		m_layout = 0;
	}
	if (m_pixelShader) {
		m_pixelShader->Release();
		m_pixelShader = 0;
	}
	if (m_vertexShader) {
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	return;
}

void FontShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename) {
	char* compileErrors;
	unsigned __int64 bufferSize;
	ofstream fout;

	//获取指向错误消息文本缓冲区的指针
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	//获取消息的长度
	bufferSize = errorMessage->GetBufferSize();

	//将错误信息写入文件
	fout.open("shader_error.txt");

	for (int i = 0; i < bufferSize; i++) {
		fout << compileErrors[i];
	}

	fout.close();

	errorMessage->Release();
	errorMessage = 0;

	MessageBox(hwnd, L"着色器编译错误，请检查shader_error.txt的消息。", shaderFilename, MB_OK);

	return;
}

bool FontShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT4 pixelColor) {
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ConstantBufferType* dataPtr;
	PixelBufferType* dataPtr2;
	unsigned int bufferNumber;

	//确保在将矩阵发送到着色器之前进行转置，这是DirectX 11的一项要求
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	//锁定可以写入的常量缓冲区
	result = deviceContext->Map(m_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) {
		return false;
	}

	//获取指向常量缓冲区中数据的指针
	dataPtr = (ConstantBufferType*)mappedResource.pData;

	//将矩阵复制到常量缓冲区中
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	//解锁常量缓冲区
	deviceContext->Unmap(m_constantBuffer, 0);

	//设置顶点着色器中常量缓冲区的位置
	bufferNumber = 0;

	//最后使用更新的值在顶点着色器中设置常量缓冲区
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_constantBuffer);

	deviceContext->PSSetShaderResources(0, 1, &texture);

	//锁定像素常量缓冲区，以便写入
	result = deviceContext->Map(m_pixelBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) {
		return false;
	}

	//获取指向像素常量缓冲区中数据的指针
	dataPtr2 = (PixelBufferType*)mappedResource.pData;

	//将像素颜色复制到常量缓冲区中
	dataPtr2->pixelColor = pixelColor;

	//解锁常量缓冲区
	deviceContext->Unmap(m_pixelBuffer, 0);

	//在像素着色器中设置像素常量缓冲区的位置
	bufferNumber = 0;

	//最后用更新后的值设置像素着色器中的像素常量缓冲区
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_pixelBuffer);

	return true;
}

void FontShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount) {
	//设置顶点输入布局
	deviceContext->IASetInputLayout(m_layout);

	//设置将用于渲染的顶点和像素着色器
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	//在像素着色器中设置采样器状态
	deviceContext->PSSetSamplers(0, 1, &m_samplerState);

	//渲染三角形
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}