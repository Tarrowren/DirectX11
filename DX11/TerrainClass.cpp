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

	//从设置文件中获取地形文件名，尺寸等等
	result = LoadSetupFile(setupFilename);
	if (!result) {
		return false;
	}

	//用位图文件中的数据初始化地形高度图
	result = LoadBitmapHeightMap();
	if (!result) {
		return false;
	}

	//设置高度图的X和Z坐标以及通过高度比例值缩放地形高度
	SetTerrainCoordinates();

	//构建地形的3D模型
	result = BuildTerrainModel();
	if (!result) {
		return false;
	}

	ShutdownHeightMap();

	//用地形数据加载渲染缓冲区
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
	//将顶点和索引缓冲区放在图形管道上，为绘制做好准备
	RenderBuffers(deviceContext);

	return true;
}

int TerrainClass::GetIndexCount() {
	//返回地形网格中的索引数量
	return m_indexCount;
}

bool TerrainClass::LoadSetupFile(char* filename) {
	int stringLength;
	ifstream fin;
	char input;

	//初始化保存地形文件名的字符串
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
	//读取地形文件名称
	fin >> m_terrainFilename;

	fin.get(input);
	while (input != ':') {
		fin.get(input);
	}
	//读取地形高度。
	fin >> m_terrainHeight;

	fin.get(input);
	while (input != ':') {
		fin.get(input);
	}
	//读取地形宽度。
	fin >> m_terrainWidth;

	fin.get(input);
	while (input != ':') {
		fin.get(input);
	}
	//读取地形高度缩放。
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

	//创建数组结构来保存高度图数据
	m_heightMap = new HeightMapType[m_terrainWidth* m_terrainHeight];
	if (!m_heightMap) {
		return false;
	}

	//以二进制打开位图映射文件
	error = fopen_s(&filePtr, m_terrainFilename, "rb");
	if (error != 0) {
		return false;
	}

	//读取位图文件标题
	count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	if (count != 1) {
		return false;
	}

	//读取位图信息标题
	count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	if (count != 1) {
		return false;
	}

	//确保高度贴图尺寸与地形尺寸相同，以便进行1对1映射
	if ((bitmapInfoHeader.biHeight != m_terrainHeight) || (bitmapInfoHeader.biWidth != m_terrainWidth)) {
		return false;
	}

	//计算位图图像数据的大小
	imageSize = m_terrainHeight * (m_terrainWidth * 3 + 1);

	bitmapImage = new unsigned char[imageSize];
	if (!bitmapImage) {
		return false;
	}

	//移动到位图数据的开头
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	//读取位图图像数据
	count = fread(bitmapImage, 1, imageSize, filePtr);
	if (count != imageSize) {
		return false;
	}

	error = fclose(filePtr);
	if (error != 0) {
		return false;
	}

	//将图像数据读入高度图阵列
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

	//计算三维地形模型中的顶点数量
	m_vertexCount = (m_terrainHeight - 1)*(m_terrainWidth - 1) * 6;

	//创建3D地形模型数组
	m_terrainModel = new ModelType[m_vertexCount];
	if (!m_terrainModel) {
		return false;
	}

	//将索引初始化为高度图数组
	index = 0;

	//使用高度图地形数据加载三维地形模型。为每个四边形创建两个三角形
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

	//设置地形网格的颜色
	color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	//计算地形中的顶点数量
	m_vertexCount = (m_terrainWidth - 1) * (m_terrainHeight - 1) * 6;

	//将索引计数设置为与顶点计数相同
	m_indexCount = m_vertexCount;

	//创建顶点数组
	vertices = new VertexType[m_vertexCount];
	if (!vertices) {
		return false;
	}

	//创建索引数组
	indices = new unsigned long[m_indexCount];
	if (!indices) {
		return false;
	}

	//使用三维地形模型数据加载顶点数组和索引数组
	for (int i = 0; i < m_vertexCount; i++) {
		vertices[i].position = XMFLOAT3(m_terrainModel[i].x, m_terrainModel[i].y, m_terrainModel[i].z);
		vertices[i].color = color;
		indices[i] = i;
	}

	//设置静态顶点缓冲区的描述
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	//给子资源结构提供一个指向顶点数据的指针
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	//现在创建顶点缓冲区
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result)) {
		return false;
	}

	//设置静态索引缓冲区的描述
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	//给子资源结构提供一个指向索引数据的指针
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	//创建索引缓冲区
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

	//设置顶点缓冲区跨度和偏移量
	stride = sizeof(VertexType);
	offset = 0;

	//将输入汇编器中的顶点缓冲区设置为活动状态，以便渲染它
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	//将输入汇编器中的索引缓冲区设置为活动状态，以便渲染它
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//设置应该从这个顶点缓冲区渲染的基本类型
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}