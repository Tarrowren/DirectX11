#include"TerrainClass.h"

TerrainClass::TerrainClass() {
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
}

TerrainClass::~TerrainClass() {

}

bool TerrainClass::Initialize(ID3D11Device* device) {
	bool result;

	//用地形数据加载渲染缓冲区
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
	//将顶点和索引缓冲区放在图形管道上，为绘制做好准备
	RenderBuffers(deviceContext);

	return true;
}

int TerrainClass::GetIndexCount() {
	//返回地形网格中的索引数量
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

	//设置地形网格的高度和宽度
	terrainHeight = 256;
	terrainWidth = 256;

	//设置地形网格的颜色
	color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	//计算地形中的顶点数量
	m_vertexCount = (terrainWidth - 1) * (terrainHeight - 1) * 8;

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

	//将索引初始化为顶点和索引数组
	index = 0;

	//用数据加载顶点数组和索引数组
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
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	return;
}