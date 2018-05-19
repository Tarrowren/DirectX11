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

	//��ʼ�������������ɫ��
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

	//���ý�������Ⱦ����ɫ������
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
	if (!result) {
		return false;
	}

	//����ɫ����Ⱦ׼���õĻ�����
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

	//���붥����ɫ������
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
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
	result = D3DCompileFromFile(psFilename, NULL, NULL, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
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
	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

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

	return true;
}

void ColorShaderClass::ShutdownShader() {
	//�ͷž����������������֡�������ɫ���Ͷ�����ɫ��
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

	//��ȡָ�������Ϣ�ı���������ָ��
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	//��ȡ��Ϣ�ĳ���
	bufferSize = errorMessage->GetBufferSize();

	//��������Ϣд���ļ�
	fout.open("shader_error.txt");

	for (i = 0; i < bufferSize; i++) {
		fout << compileErrors[i];
	}

	fout.close();

	errorMessage->Release();
	errorMessage = 0;

	MessageBox(hwnd, L"��ɫ�������������shader_error.txt����Ϣ��", shaderFilename, MB_OK);

	return;
}

bool ColorShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix) {
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
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

	return true;
}

void ColorShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount) {
	//���ö������벼��
	deviceContext->IASetInputLayout(m_layout);

	//���ý�������Ⱦ�Ķ����������ɫ��
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	//��ʼ��Ⱦ
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}