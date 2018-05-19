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

	//加载模型数据
	result = LoadModel(modelFilename);
	if (!result) {
		return false;
	}

	//初始化顶点和索引缓冲区
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
	//将顶点和索引缓冲区放在图形管道上，为绘制做好准备
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

	//创建顶点和索引数组
	vertices = new VertexType[m_vertexCount];
	if (!vertices) {
		return false;
	}

	indices = new unsigned long[m_indexCount];
	if (!indices) {
		return false;
	}

	//将顶点发送到GPU的顺序非常重要
	for (int i = 0; i < m_vertexCount; i++) {
		vertices[i].position = XMFLOAT3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].color = XMFLOAT4(m_model[i].a, m_model[i].b, m_model[i].c, m_model[i].d);

		indices[i] = i;
	}
	
	//设置静态顶点缓冲区的描述
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType)* m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	//给子资源结构提供一个指向顶点数据的指针
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	//创建顶点缓冲区
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result)) {
		return false;
	}

	//设置静态索引缓冲区的描述
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long)* m_indexCount;
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

void ModelClass::ShutdownBuffers() {
	//释放索引、顶点缓冲区
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

bool ModelClass::LoadModel(WCHAR* filename) {
	ifstream fin;
	char input;
	
	fin.open(filename);
	if (fin.fail()) {
		return false;
	}

	//读取顶点数的值
	fin.get(input);
	while (input != ':') {
		fin.get(input);
	}

	//读入顶点数
	fin >> m_vertexCount;

	//设置索引的数量与顶点数量相同
	m_indexCount = m_vertexCount;

	//使用读入的顶点数量创建模型
	m_model = new ModelType[m_vertexCount];
	if (!m_model) {
		return false;
	}

	//读到数据的开头
	fin.get(input);
	while (input != ':') {
		fin.get(input);
	}

	//读取顶点数据
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