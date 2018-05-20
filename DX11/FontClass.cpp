#include"FontClass.h"

FontClass::FontClass() {
	m_Font = 0;
	m_Texture = 0;
}

FontClass::~FontClass() {

}

bool FontClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* fontFilename, char* textureFilename) {
	bool result;

	result = LoadFontData(fontFilename);
	if (!result) {
		return false;
	}
	result = LoadTexture(device, deviceContext, textureFilename);
	if (!result) {
		return false;
	}

	return true;
}

void FontClass::Shutdown() {
	ReleaseTexture();
	ReleaseFontData();

	return;
}

bool FontClass::LoadFontData(char* filename) {
	ifstream fin;
	char temp;

	//创建字体间距缓冲区
	m_Font = new FontType[95];
	if (!m_Font) {
		return false;
	}

	//读取字符之间的字体大小和间距
	fin.open(filename);
	if (fin.fail()) {
		return false;
	}

	//读取95个用于文本的ascii字符
	for (int i = 0; i < 95; i++) {
		fin.get(temp);
		while (temp != ' ') {
			fin.get(temp);
		}
		fin.get(temp);
		while (temp != ' ') {
			fin.get(temp);
		}

		fin >> m_Font[i].left;
		fin >> m_Font[i].right;
		fin >> m_Font[i].size;
	}

	fin.close();

	return true;
}

void FontClass::ReleaseFontData() {
	if (m_Font) {
		delete[] m_Font;
		m_Font = 0;
	}

	return;
}

bool FontClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename) {
	bool result;

	m_Texture = new TextureClass;
	if (!m_Texture) {
		return false;
	}
	result = m_Texture->Initialize(device, deviceContext, filename);
	if (!result) {
		return false;
	}

	return true;
}

void FontClass::ReleaseTexture() {
	if (m_Texture) {
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}

ID3D11ShaderResourceView* FontClass::GetTexture() {
	return m_Texture->GetTexture();
}

void FontClass::BuildVertexArray(void* vertices, char* sentence, float drawX, float drawY) {
	VertexType* vertexPtr;
	int numLetters, index, letter;

	//将输入顶点强制为VertexType结构
	vertexPtr = (VertexType*)vertices;

	//获取句子中的字母数量
	numLetters = (int)strlen(sentence);

	//初始化顶点数组的索引
	index = 0;

	//将每个字母绘制到四边形上
	for (int i = 0; i < numLetters; i++) {
		letter = (int)sentence[i] - 32;

		//如果字母是空格，那么只需移动三个像素即可
		if (letter == 0) {
			drawX = drawX + 3.0f;
		}
		else {
			//第一个三角形
			vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f);
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3(drawX + m_Font[letter].size, drawY - 16, 0.0f);
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 1.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3(drawX, drawY - 16, 0.0f);
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 1.0f);
			index++;

			//第二个三角形
			vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f);
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3(drawX + m_Font[letter].size, drawY, 0.0f);
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 0.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3(drawX + m_Font[letter].size, drawY - 16, 0.0f);
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 1.0f);
			index++;

			drawX = drawX + m_Font[letter].size + 1.0f;
		}
	}

	return;
}