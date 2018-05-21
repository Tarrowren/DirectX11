#pragma once

#include"D3DClass.h"
#include"ColorShaderClass.h"
#include"FontShaderClass.h"

class ShaderManagerClass {

public:

	ShaderManagerClass();
	~ShaderManagerClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool RenderColorShader(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX);
	bool RenderFontShader(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT4);

private:

	ColorShaderClass * m_ColorShader;
	FontShaderClass* m_FontShader;

};