#include"ColorShaderClass.h"

ColorShaderClass::ColorShaderClass() {
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
}

ColorShaderClass::~ColorShaderClass() {

}

bool ColorShaderClass::Initialize(ID3D11Device *device, HWND hwnd) {
	bool result;

	//初始化顶点和像素着色器
	result = InitializeShader(device, hwnd, (WCHAR*)L"VertexShader.hlsl", (WCHAR*)L"PixelShader.hlsl");
	if (!result) {
		return false;
	}

	return true;
}

void ColorShaderClass::Shutdown() {
	ShutdownShader();

	return;
}

bool ColorShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix) {
	bool result;

	//设置将用于渲染的着色器参数
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
	if (!result) {
		return false;
	}

	//用着色器渲染准备好的缓冲区
	RenderShader(deviceContext, indexCount);

	return true;
}

bool ColorShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename) {
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;

	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	//编译顶点着色器代码
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result)) {
		//如果着色器编译失败，它应该写入错误信息
		if (errorMessage) {
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		//如果错误消息中没有任何内容，那么就是找不到着色器文件
		else {
			MessageBox(hwnd, vsFilename, L"缺少顶点着色器文件", MB_OK);
		}

		return false;
	}

	//编译像素着色器代码
	result = D3DCompileFromFile(psFilename, NULL, NULL, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result)) {
		//如果着色器编译失败，它应该写入错误信息
		if (errorMessage) {
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		//如果错误消息中没有任何内容，那么就是找不到着色器文件
		else {
			MessageBox(hwnd, psFilename, L"缺少像素着色器文件", MB_OK);
		}

		return false;
	}

	//从缓冲区创建顶点和像素着色器
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result)) {
		return false;
	}

	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result)) {
		return false;
	}

	//创建顶点输入布局描述
	//此设置与ModelClass和着色器中的VertexType结构匹配
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	//获取布局中元素的数量
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	//创建顶点输入布局
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result)) {
		return false;
	}

	//释放顶点和像素着色器缓冲区
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;
	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	//设置顶点着色器中动态矩阵常量缓冲区的描述
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	//创建常量缓冲区指针，以便我们可以从该类中访问顶点着色器常量缓冲区
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result)) {
		return false;
	}

	return true;
}

void ColorShaderClass::ShutdownShader() {
	//释放矩阵常量缓冲区、布局、像素着色器和顶点着色器
	if (m_matrixBuffer) {
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
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

void ColorShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename) {
	char* compileErrors;
	unsigned long long bufferSize, i;
	ofstream fout;

	//获取指向错误消息文本缓冲区的指针
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	//获取消息的长度
	bufferSize = errorMessage->GetBufferSize();

	//将错误信息写入文件
	fout.open("shader_error.txt");

	for (i = 0; i < bufferSize; i++) {
		fout << compileErrors[i];
	}

	fout.close();

	errorMessage->Release();
	errorMessage = 0;

	MessageBox(hwnd, L"着色器编译错误，请检查shader_error.txt的消息。", shaderFilename, MB_OK);

	return;
}

bool ColorShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix) {
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	//确保在将矩阵发送到着色器之前进行转置，这是DirectX 11的一项要求
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	//锁定可以写入的常量缓冲区
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) {
		return false;
	}

	//获取指向常量缓冲区中数据的指针
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	//将矩阵复制到常量缓冲区中
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	//解锁常量缓冲区
	deviceContext->Unmap(m_matrixBuffer, 0);

	//设置顶点着色器中常量缓冲区的位置
	bufferNumber = 0;

	//最后使用更新的值在顶点着色器中设置常量缓冲区
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	return true;
}

void ColorShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount) {
	//设置顶点输入布局
	deviceContext->IASetInputLayout(m_layout);

	//设置将用于渲染的顶点和像素着色器
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	//开始渲染
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}