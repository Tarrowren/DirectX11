#pragma once
#include<d3d11.h>
#include<DirectXMath.h>
#include<fstream>
#include<stdio.h>

using namespace DirectX;
using namespace std;

class TerrainClass {

private:

	struct VertexType {
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

	struct HeightMapType{
		float x, y, z;
	};

	struct ModelType{
		float x, y, z;
	};

public:

	TerrainClass();
	~TerrainClass();

	bool Initialize(ID3D11Device*, char*);
	void Shutdown();
	bool Render(ID3D11DeviceContext*);

	int GetIndexCount();

private:

	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadSetupFile(char*);

	bool LoadBitmapHeightMap();
	void ShutdownHeightMap();

	void SetTerrainCoordinates();

	bool BuildTerrainModel();
	void ShutdownTerrainModel();

private:

	ID3D11Buffer * m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;

	//地图尺寸
	int m_terrainHeight, m_terrainWidth;
	float m_heightScale;

	char* m_terrainFilename;//地图位图文件名
	HeightMapType* m_heightMap;//高度图数组
	ModelType* m_terrainModel;//三维地形模型数组

};