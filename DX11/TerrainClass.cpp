#include"TerrainClass.h"

TerrainClass::TerrainClass() {
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
}

TerrainClass::~TerrainClass() {

}

bool TerrainClass::Initialize(ID3D11Device* device) {
	bool result;

	//�õ������ݼ�����Ⱦ������
	result = InitializeBuffers(device);
	if (!result) {
		return false;
	}

	return true;
}

void TerrainClass::Shutdown() {
	ShutdownBuffers();

	return;
}

bool TerrainClass::Render(ID3D11DeviceContext* deviceContext) {
	//���������������������ͼ�ιܵ��ϣ�Ϊ��������׼��
	RenderBuffers(deviceContext);

	return true;
}

int TerrainClass::GetIndexCount() {
	//���ص��������е���������
	return m_indexCount;
}

bool TerrainClass::InitializeBuffers(ID3D11Device* device) {
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int terrainWidth, terrainHeight, index;
	XMFLOAT4 color;
	float positionX, positionZ;

	//���õ�������ĸ߶ȺͿ��
	terrainHeight = 256;
	terrainWidth = 256;

	//���õ����������ɫ
	color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	//��������еĶ�������
	m_vertexCount = (terrainWidth - 1) * (terrainHeight - 1) * 8;

	//��������������Ϊ�붥�������ͬ
	m_indexCount = m_vertexCount;

	//������������
	vertices = new VertexType[m_vertexCount];
	if (!vertices) {
		return false;
	}

	//������������
	indices = new unsigned long[m_indexCount];
	if (!indices) {
		return false;
	}

	//��������ʼ��Ϊ�������������
	index = 0;

	//�����ݼ��ض����������������
	for (int j = 0; j<(terrainHeight - 1); j++) {
		for (int i = 0; i<(terrainWidth - 1); i++) {
			positionX = (float)i;
			positionZ = (float)(j + 1);
			vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			positionX = (float)(i + 1);
			positionZ = (float)(j + 1);
			vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			positionX = (float)(i + 1);
			positionZ = (float)(j + 1);
			vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			positionX = (float)(i + 1);
			positionZ = (float)j;
			vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			positionX = (float)(i + 1);
			positionZ = (float)j;
			vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			positionX = (float)i;
			positionZ = (float)j;
			vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			positionX = (float)i;
			positionZ = (float)j;
			vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			positionX = (float)i;
			positionZ = (float)(j + 1);
			vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[index].color = color;
			indices[index] = index;
			index++;
		}
	}

	//���þ�̬���㻺����������
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	//������Դ�ṹ�ṩһ��ָ�򶥵����ݵ�ָ��
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	//���ڴ������㻺����
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result)) {
		return false;
	}

	//���þ�̬����������������
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
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

void TerrainClass::ShutdownBuffers() {
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

void TerrainClass::RenderBuffers(ID3D11DeviceContext* deviceContext) {
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
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	return;
}