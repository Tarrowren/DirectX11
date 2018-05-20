#include"LightShaderClass.h"

LightShaderClass::LightShaderClass() {
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_sampleState = 0;
	m_matrixBuffer = 0;
	m_lightBuffer = 0;
}

LightShaderClass::~LightShaderClass() {

}

bool LightShaderClass::Initialize(ID3D11Device *device, HWND hwnd) {
	bool result;

	//��ʼ�������������ɫ��
	result = InitializeShader(device, hwnd, (WCHAR*)L"LightVertexShader.hlsl", (WCHAR*)L"LightPixelShader.hlsl");
	if (!result) {
		return false;
	}

	return true;
}

void LightShaderClass::Shutdown() {
	ShutdownShader();

	return;
}

bool LightShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor) {
	bool result;

	//���ý�������Ⱦ����ɫ������
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, lightDirection, diffuseColor);
	if (!result) {
		return false;
	}

	//����ɫ����Ⱦ׼���õĻ�����
	RenderShader(deviceContext, indexCount);

	return true;
}

bool LightShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename) {
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	unsigned int numElements;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;

	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	//���붥����ɫ������
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "LightVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result)) {
		//�����ɫ������ʧ�ܣ���Ӧ��д�������Ϣ
		if (errorMessage) {
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		//���������Ϣ��û���κ����ݣ���ô�����Ҳ�����ɫ���ļ�
		else {
			MessageBox(hwnd, vsFilename, L"ȱ�ٶ�����ɫ���ļ�", MB_OK);
		}

		return false;
	}

	//����������ɫ������
	result = D3DCompileFromFile(psFilename, NULL, NULL, "LightPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result)) {
		//�����ɫ������ʧ�ܣ���Ӧ��д�������Ϣ
		if (errorMessage) {
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		//���������Ϣ��û���κ����ݣ���ô�����Ҳ�����ɫ���ļ�
		else {
			MessageBox(hwnd, psFilename, L"ȱ��������ɫ���ļ�", MB_OK);
		}

		return false;
	}

	//�ӻ��������������������ɫ��
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result)) {
		return false;
	}

	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result)) {
		return false;
	}

	//�����������벼������
	//��������ModelClass����ɫ���е�VertexType�ṹƥ��
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

	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	//��ȡ������Ԫ�ص�����
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	//�����������벼��
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result)) {
		return false;
	}

	//�ͷŶ����������ɫ��������
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;
	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	//���ö�����ɫ���ж�̬������������������
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	//��������������ָ�룬�Ա����ǿ��ԴӸ����з��ʶ�����ɫ������������
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result)) {
		return false;
	}

	//����һ�����������״̬����
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

	//�������������״̬
	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result)) {
		return false;
	}

	//����������ɫ���еĹ⶯̬����������������
	//��ע�⣬���ʹ��D3D11_BIND_CONSTANT_BUFFER��CreateBufferʧ�ܣ���ByteWidth������ҪΪ16�ı���
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	//��������������ָ�룬�Ա����ǿ��ԴӸ����з��ʶ�����ɫ������������
	result = device->CreateBuffer(&lightBufferDesc, NULL, &m_lightBuffer);
	if (FAILED(result)) {
		return false;
	}

	return true;
}

void LightShaderClass::ShutdownShader() {
	if (m_lightBuffer) {
		m_lightBuffer->Release();
		m_lightBuffer = 0;
	}
	if (m_matrixBuffer) {
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}
	if (m_sampleState) {
		m_sampleState->Release();
		m_sampleState = 0;
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

void LightShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename) {
	char* compileErrors;
	unsigned long long bufferSize;
	ofstream fout;

	//��ȡָ�������Ϣ�ı���������ָ��
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	//��ȡ��Ϣ�ĳ���
	bufferSize = errorMessage->GetBufferSize();

	//��������Ϣд���ļ�
	fout.open("shader_error.txt");

	for (int i = 0; i < bufferSize; i++) {
		fout << compileErrors[i];
	}

	fout.close();

	errorMessage->Release();
	errorMessage = 0;

	MessageBox(hwnd, L"��ɫ�������������shader_error.txt����Ϣ��", shaderFilename, MB_OK);

	return;
}

bool LightShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor) {
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	LightBufferType* dataPtr2;
	unsigned int bufferNumber;

	//ȷ���ڽ������͵���ɫ��֮ǰ����ת�ã�����DirectX 11��һ��Ҫ��
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	//��������д��ĳ���������
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) {
		return false;
	}

	//��ȡָ���������������ݵ�ָ��
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	//�������Ƶ�������������
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	//��������������
	deviceContext->Unmap(m_matrixBuffer, 0);

	//���ö�����ɫ���г�����������λ��
	bufferNumber = 0;

	//���ʹ�ø��µ�ֵ�ڶ�����ɫ�������ó���������
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	deviceContext->PSSetShaderResources(0, 1, &texture);

	//�����ⳣ�����������Ա�д��
	result = deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) {
		return false;
	}

	//��ȡָ���������������ݵ�ָ��
	dataPtr2 = (LightBufferType*)mappedResource.pData;

	//�������������Ƶ�������������
	dataPtr2->diffuseColor = diffuseColor;
	dataPtr2->lightDirection = lightDirection;
	dataPtr2->padding = 0.0f;

	//��������������
	deviceContext->Unmap(m_lightBuffer, 0);

	//��������ɫ�������ùⳣ����������λ��
	bufferNumber = 0;

	//����ø��º��ֵ����������ɫ���еĹⳣ��������
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);

	return true;
}

void LightShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount) {
	//���ö������벼��
	deviceContext->IASetInputLayout(m_layout);

	//���ý�������Ⱦ�Ķ����������ɫ��
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	//��������ɫ�������ò�����״̬
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	//��Ⱦ������
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}