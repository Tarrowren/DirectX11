#pragma once
#include<d3d11.h>
#include<DirectXMath.h>
#include<fstream>

using namespace DirectX;
using namespace std;

class ModelClass {

private:

	//这里是顶点类型的定义，它将与ModelClass中的顶点缓冲区一起使用。还要注意，这个类型定义必须与ColorShaderClass中的布局相匹配
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

	//模型定义
	struct ModelType {
		float x, y, z;
		float a, b, c, d;
	};

public:

	ModelClass();
	~ModelClass();

	bool Initialize(ID3D11Device*, WCHAR*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();

private:

	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadModel(WCHAR*);
	void ReleaseModel();

private:

	ID3D11Buffer * m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
	ModelType* m_model;
};