#include"TerrainClass.h"

TerrainClass::TerrainClass() {
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_terrainFilename = 0;
	m_heightMap = 0;
	m_terrainModel = 0;
}

TerrainClass::~TerrainClass() {

}

bool TerrainClass::Initialize(ID3D11Device* device, char* setupFilename) {
	bool result;

	//�������ļ��л�ȡ�����ļ������ߴ�ȵ�
	result = LoadSetupFile(setupFilename);
	if (!result) {
		return false;
	}

	//��λͼ�ļ��е����ݳ�ʼ�����θ߶�ͼ
	result = LoadBitmapHeightMap();
	if (!result) {
		return false;
	}

	//���ø߶�ͼ��X��Z�����Լ�ͨ���߶ȱ���ֵ���ŵ��θ߶�
	SetTerrainCoordinates();

	//�������ε�3Dģ��
	result = BuildTerrainModel();
	if (!result) {
		return false;
	}

	ShutdownHeightMap();

	//�õ������ݼ�����Ⱦ������
	result = InitializeBuffers(device);
	if (!result) {
		return false;
	}

	ShutdownTerrainModel();

	return true;
}

void TerrainClass::Shutdown() {
	ShutdownBuffers();
	ShutdownHeightMap();
	ShutdownTerrainModel();

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

bool TerrainClass::LoadSetupFile(char* filename) {
	int stringLength;
	ifstream fin;
	char input;

	//��ʼ����������ļ������ַ���
	stringLength = 256;
	m_terrainFilename = new char[stringLength];
	if (!m_terrainFilename) {
		return false;
	}

	fin.open(filename);
	if (fin.fail()) {
		return false;
	}

	fin.get(input);
	while (input != ':') {
		fin.get(input);
	}
	//��ȡ�����ļ�����
	fin >> m_terrainFilename;

	fin.get(input);
	while (input != ':') {
		fin.get(input);
	}
	//��ȡ���θ߶ȡ�
	fin >> m_terrainHeight;

	fin.get(input);
	while (input != ':') {
		fin.get(input);
	}
	//��ȡ���ο�ȡ�
	fin >> m_terrainWidth;

	fin.get(input);
	while (input != ':') {
		fin.get(input);
	}
	//��ȡ���θ߶����š�
	fin >> m_heightScale;

	fin.close();

	return true;
}

bool TerrainClass::LoadBitmapHeightMap() {
	int error, imageSize, k, index;
	FILE* filePtr;
	unsigned long long count;
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	unsigned char* bitmapImage;
	unsigned char height;

	//��������ṹ������߶�ͼ����
	m_heightMap = new HeightMapType[m_terrainWidth* m_terrainHeight];
	if (!m_heightMap) {
		return false;
	}

	//�Զ����ƴ�λͼӳ���ļ�
	error = fopen_s(&filePtr, m_terrainFilename, "rb");
	if (error != 0) {
		return false;
	}

	//��ȡλͼ�ļ�����
	count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	if (count != 1) {
		return false;
	}

	//��ȡλͼ��Ϣ����
	count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	if (count != 1) {
		return false;
	}

	//ȷ���߶���ͼ�ߴ�����γߴ���ͬ���Ա����1��1ӳ��
	if ((bitmapInfoHeader.biHeight != m_terrainHeight) || (bitmapInfoHeader.biWidth != m_terrainWidth)) {
		return false;
	}

	//����λͼͼ�����ݵĴ�С
	imageSize = m_terrainHeight * (m_terrainWidth * 3 + 1);

	bitmapImage = new unsigned char[imageSize];
	if (!bitmapImage) {
		return false;
	}

	//�ƶ���λͼ���ݵĿ�ͷ
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	//��ȡλͼͼ������
	count = fread(bitmapImage, 1, imageSize, filePtr);
	if (count != imageSize) {
		return false;
	}

	error = fclose(filePtr);
	if (error != 0) {
		return false;
	}

	//��ͼ�����ݶ���߶�ͼ����
	k = 0;
	for (int j = 0; j < m_terrainHeight; j++) {
		for (int i = 0; i < m_terrainWidth; i++) {
			index = m_terrainWidth * (m_terrainHeight - 1 - j) + i;
			height = bitmapImage[k];
			m_heightMap[index].y = (float)height;
			k += 3;
		}
		k++;
	}
	delete[] bitmapImage;
	bitmapImage = 0;

	delete[] m_terrainFilename;
	m_terrainFilename = 0;

	return true;
}

void TerrainClass::ShutdownHeightMap() {
	if (m_heightMap) {
		delete[] m_heightMap;
		m_heightMap = 0;
	}

	return;
}

void TerrainClass::SetTerrainCoordinates() {
	int index;
	
	for (int j = 0; j < m_terrainHeight; j++) {
		for (int i = 0; i < m_terrainWidth; i++) {
			index = (m_terrainWidth*j) + i;

			m_heightMap[index].x = (float)i;
			m_heightMap[index].z = (float)(m_terrainHeight - 1 - j);
			m_heightMap[index].y /= m_heightScale;
		}
	}

	return;
}

bool TerrainClass::BuildTerrainModel() {
	int index, index1, index2, index3, index4;

	//������ά����ģ���еĶ�������
	m_vertexCount = (m_terrainHeight - 1)*(m_terrainWidth - 1) * 6;

	//����3D����ģ������
	m_terrainModel = new ModelType[m_vertexCount];
	if (!m_terrainModel) {
		return false;
	}

	//��������ʼ��Ϊ�߶�ͼ����
	index = 0;

	//ʹ�ø߶�ͼ�������ݼ�����ά����ģ�͡�Ϊÿ���ı��δ�������������
	for (int j = 0; j < m_terrainHeight - 1; j++) {
		for (int i = 0; i < m_terrainWidth - 1; i++) {
			index1 = (m_terrainWidth * j) + i;
			index2 = (m_terrainWidth * j) + (i + 1);
			index3 = (m_terrainWidth * (j + 1)) + i;
			index4 = (m_terrainWidth * (j + 1)) + (i + 1);

			m_terrainModel[index].x = m_heightMap[index1].x;
			m_terrainModel[index].y = m_heightMap[index1].y;
			m_terrainModel[index].z = m_heightMap[index1].z;
			index++;

			m_terrainModel[index].x = m_heightMap[index2].x;
			m_terrainModel[index].y = m_heightMap[index2].y;
			m_terrainModel[index].z = m_heightMap[index2].z;
			index++;

			m_terrainModel[index].x = m_heightMap[index3].x;
			m_terrainModel[index].y = m_heightMap[index3].y;
			m_terrainModel[index].z = m_heightMap[index3].z;
			index++;

			m_terrainModel[index].x = m_heightMap[index3].x;
			m_terrainModel[index].y = m_heightMap[index3].y;
			m_terrainModel[index].z = m_heightMap[index3].z;
			index++;

			m_terrainModel[index].x = m_heightMap[index2].x;
			m_terrainModel[index].y = m_heightMap[index2].y;
			m_terrainModel[index].z = m_heightMap[index2].z;
			index++;

			m_terrainModel[index].x = m_heightMap[index4].x;
			m_terrainModel[index].y = m_heightMap[index4].y;
			m_terrainModel[index].z = m_heightMap[index4].z;
			index++;
		}
	}

	return true;
}

void TerrainClass::ShutdownTerrainModel() {
	if (m_terrainModel) {
		delete[] m_terrainModel;
		m_terrainModel = 0;
	}

	return;
}

bool TerrainClass::InitializeBuffers(ID3D11Device* device) {
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	XMFLOAT4 color;

	//���õ����������ɫ
	color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	//��������еĶ�������
	m_vertexCount = (m_terrainWidth - 1) * (m_terrainHeight - 1) * 6;

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

	//ʹ����ά����ģ�����ݼ��ض����������������
	for (int i = 0; i < m_vertexCount; i++) {
		vertices[i].position = XMFLOAT3(m_terrainModel[i].x, m_terrainModel[i].y, m_terrainModel[i].z);
		vertices[i].color = color;
		indices[i] = i;
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
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}