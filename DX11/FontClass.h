#pragma once
#include<DirectXMath.h>
#include<fstream>

#include"TextureClass.h"

using namespace DirectX;
using namespace std;

class FontClass {

private:

	struct FontType
	{
		float left, right;
		int size;
	};

	struct VertexType {
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

public:

	FontClass();
	~FontClass();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*, char*, float, int);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();
	void BuildVertexArray(void*, char*, float, float);
	int GetSentencePixelLength(char*);
	int GetFontHeight();

private:

	bool LoadFontData(char*);
	void ReleaseFontData();
	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*);
	void ReleaseTexture();

private:

	FontType * m_Font;
	TextureClass* m_Texture;
	float m_fontHeight;
	int m_spaceSize;

};