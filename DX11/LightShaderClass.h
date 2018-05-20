#pragma once
#include<d3d11.h>
#include<d3dcompiler.h>
#include<DirectXMath.h>
#include<fstream>

using namespace DirectX;
using namespace std;

class LightShaderClass {

private:

	//以下是将与顶点着色器一起使用的cBuffer类型的定义。这个类型定义必须与顶点着色器中的完全相同。
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	struct LightBufferType {
		XMFLOAT4 diffuseColor;
		XMFLOAT3 lightDirection;
		float padding;
	};

public:

	LightShaderClass();
	~LightShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	//渲染函数设置着色器参数，然后使用着色器绘制准备好的模型顶点。
	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT3, XMFLOAT4);

private:

	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT3, XMFLOAT4);
	void RenderShader(ID3D11DeviceContext*, int);

private:

	ID3D11VertexShader * m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleState;//该指针将用于与纹理着色器进行交互
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer;//该类将使用光缓冲区来设置HLSL像素着色器内的全局光变量

};