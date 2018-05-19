#include"ModelClass.h"

ModelClass::ModelClass() {
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_model = 0;
}

ModelClass::~ModelClass() {

}

bool ModelClass::Initialize(ID3D11Device* device,WCHAR* modelFilename) {
	bool result;

	//����ģ������
	result = LoadModel(modelFilename);
	if (!result) {
		return false;
	}

	//��ʼ�����������������
	result = InitializeBuffers(device);
	if (!result) {
		return false;
	}

	return true;
}

void ModelClass::Shutdown() {
	ShutdownBuffers();
	ReleaseModel();

	return;
}

void ModelClass::Render(ID3D11DeviceContext* deviceContext) {
	//���������������������ͼ�ιܵ��ϣ�Ϊ��������׼��
	RenderBuffers(deviceContext);

	return;
}

int ModelClass::GetIndexCount() {
	return m_indexCount;
}

bool ModelClass::InitializeBuffers(ID3D11Device* device) {
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	//�����������������
	vertices = new VertexType[m_vertexCount];
	if (!vertices) {
		return false;
	}

	indices = new unsigned long[m_indexCount];
	if (!indices) {
		return false;
	}

	//�����㷢�͵�GPU��˳��ǳ���Ҫ
	for (int i = 0; i < m_vertexCount; i++) {
		vertices[i].position = XMFLOAT3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].color = XMFLOAT4(m_model[i].a, m_model[i].b, m_model[i].c, m_model[i].d);

		indices[i] = i;
	}
	
	//���þ�̬���㻺����������
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType)* m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	//������Դ�ṹ�ṩһ��ָ�򶥵����ݵ�ָ��
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	//�������㻺����
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result)) {
		return false;
	}

	//���þ�̬����������������
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long)* m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	//������Դ�ṹ�ṩһ��ָ���������ݵ�ָ��
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	//��������������
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result)) {
		return false;
	}

	delete[] vertices;
	vertices = 0;
	delete[] indices;
	indices = 0;

	return true;
}

void ModelClass::ShutdownBuffers() {
	//�ͷ����������㻺����
	if (m_indexBuffer) {
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}
	if (m_vertexBuffer) {
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}

void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext) {
	unsigned int stride;
	unsigned int offset;

	//���ö��㻺������Ⱥ�ƫ����
	stride = sizeof(VertexType);
	offset = 0;

	//�����������еĶ��㻺��������Ϊ�״̬���Ա���Ⱦ��
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	//�����������е���������������Ϊ�״̬���Ա���Ⱦ��
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//����Ӧ�ô�������㻺������Ⱦ�Ļ�������
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

bool ModelClass::LoadModel(WCHAR* filename) {
	ifstream fin;
	char input;
	
	fin.open(filename);
	if (fin.fail()) {
		return false;
	}

	//��ȡ��������ֵ
	fin.get(input);
	while (input != ':') {
		fin.get(input);
	}

	//���붥����
	fin >> m_vertexCount;

	//���������������붥��������ͬ
	m_indexCount = m_vertexCount;

	//ʹ�ö���Ķ�����������ģ��
	m_model = new ModelType[m_vertexCount];
	if (!m_model) {
		return false;
	}

	//�������ݵĿ�ͷ
	fin.get(input);
	while (input != ':') {
		fin.get(input);
	}

	//��ȡ��������
	for (int i = 0; i < m_vertexCount; i++) {
		fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
		fin >> m_model[i].a >> m_model[i].b >> m_model[i].c >> m_model[i].d;
	}

	fin.close();

	return true;
}

void ModelClass::ReleaseModel() {
	if (m_model) {
		delete[] m_model;
		m_model = 0;
	}

	return;
}