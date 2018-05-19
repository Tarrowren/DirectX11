#pragma once
#include<d3d11.h>
#include<DirectXMath.h>
#include<fstream>

using namespace DirectX;
using namespace std;

class ModelClass {

private:

	//�����Ƕ������͵Ķ��壬������ModelClass�еĶ��㻺����һ��ʹ�á���Ҫע�⣬������Ͷ��������ColorShaderClass�еĲ�����ƥ��
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

	//ģ�Ͷ���
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