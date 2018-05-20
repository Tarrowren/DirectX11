#include"TextClass.h"

TextClass::TextClass() {
	m_Font = 0;
	m_FontShader = 0;
	m_sentence1 = 0;
	m_sentence2 = 0;
}

TextClass::~TextClass() {

}

bool TextClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd, int screenWidth, int screenHeight, XMMATRIX baseViewMatrix) {
	bool result;

	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	m_baseViewMatrix = baseViewMatrix;

	m_Font = new FontClass;
	if (!m_Font) {
		return false;
	}
	result = m_Font->Initialize(device, deviceContext, (char*) "fontdata.txt", (char*) "font.tga");
	if (!result) {
		MessageBox(hwnd, L"无法初始化字体对象", L"错误", MB_OK);

		return false;
	}

	m_FontShader = new FontShaderClass;
	if (!m_FontShader) {
		return false;
	}
	result = m_FontShader->Initialize(device, hwnd);
	if (!result) {
		MessageBox(hwnd, L"无法初始化字体着色器对象", L"错误", MB_OK);

		return false;
	}

	result = InitializeSentence(&m_sentence1, 16, device);
	if (!result) {
		return false;
	}

	result = UpdateSentence(m_sentence1, (char*)"Fps: ", 20, 20, 0.0f, 1.0f, 0.0f, deviceContext);
	if (!result) {
		return false;
	}

	result = InitializeSentence(&m_sentence2, 16, device);
	if (!result) {
		return false;
	}

	result = UpdateSentence(m_sentence2, (char*)"Cpu: ", 20, 40, 0.0f, 1.0f, 0.0f, deviceContext);
	if (!result) {
		return false;
	}

	return true;
}

void TextClass::Shutdown() {
	ReleaseSentence(&m_sentence1);
	ReleaseSentence(&m_sentence2);
	if (m_FontShader) {
		m_FontShader->Shutdown();
		delete m_FontShader;
		m_FontShader = 0;
	}
	if (m_Font) {
		m_Font->Shutdown();
		delete m_Font;
		m_Font = 0;
	}

	return;
}

bool TextClass::Render(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX orthoMatrix) {
	bool result;

	result = RenderSentence(deviceContext, m_sentence1, worldMatrix, orthoMatrix);
	if (!result) {
		return false;
	}
	result = RenderSentence(deviceContext, m_sentence2, worldMatrix, orthoMatrix);
	if (!result) {
		return false;
	}

	return true;
}

bool TextClass::InitializeSentence(SentenceType** sentence, int maxLength, ID3D11Device* device) {
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	*sentence = new SentenceType;
	if (!*sentence) {
		return false;
	}

	(*sentence)->vertexBuffer = 0;
	(*sentence)->indexBuffer = 0;

	(*sentence)->maxLength = maxLength;

	(*sentence)->vertexCount = 6 * maxLength;

	(*sentence)->indexCount = (*sentence)->vertexCount;

	vertices = new VertexType[(*sentence)->vertexCount];
	if (!vertices) {
		return false;
	}

	indices = new unsigned long[(*sentence)->indexCount];
	if (!indices) {
		return false;
	}

	memset(vertices, 0, sizeof(VertexType)*(*sentence)->vertexCount);

	for (int i = 0; i < (*sentence)->indexCount; i++) {
		indices[i] = i;
	}

	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType)*(*sentence)->vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &(*sentence)->vertexBuffer);
	if (FAILED(result)) {
		return false;
	}

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long)*(*sentence)->indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&indexBufferDesc, &indexData, &(*sentence)->indexBuffer);
	if (FAILED(result)) {
		return false;
	}

	delete[] vertices;
	vertices = 0;
	delete[] indices;
	indices = 0;

	return true;
}

bool TextClass::UpdateSentence(SentenceType* sentence, char* text, int positionX, int positionY, float red, float green, float blue, ID3D11DeviceContext* deviceContext) {
	int numLetters;
	VertexType* vertices;
	float drawX, drawY;
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;

	sentence->red = red;
	sentence->green = green;
	sentence->blue = blue;

	numLetters = (int)strlen(text);

	if (numLetters > sentence->maxLength) {
		return false;
	}

	vertices = new VertexType[sentence->vertexCount];
	if (!vertices) {
		return false;
	}

	memset(vertices, 0, sizeof(VertexType)*sentence->vertexCount);

	drawX = (float)(positionX - m_screenWidth / 2);
	drawY = (float)(m_screenHeight / 2 - positionY);

	m_Font->BuildVertexArray((void*)vertices, text, drawX, drawY);

	result = deviceContext->Map(sentence->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) {
		return false;
	}

	verticesPtr = (VertexType*)mappedResource.pData;

	memcpy(verticesPtr, (void*)vertices, sizeof(VertexType)*sentence->vertexCount);

	deviceContext->Unmap(sentence->vertexBuffer, 0);

	delete[] vertices;
	vertices = 0;

	return true;
}

void TextClass::ReleaseSentence(SentenceType** sentence) {
	if (*sentence) {
		if ((*sentence)->vertexBuffer) {
			(*sentence)->vertexBuffer->Release();
			(*sentence)->vertexBuffer = 0;
		}
		if ((*sentence)->indexBuffer) {
			(*sentence)->indexBuffer->Release();
			(*sentence)->indexBuffer = 0;
		}

		delete* sentence;
		*sentence = 0;
	}

	return;
}

bool TextClass::RenderSentence(ID3D11DeviceContext* deviceContext, SentenceType* sentence, XMMATRIX worldMatrix, XMMATRIX orthoMatrix) {
	unsigned int stride, offset;
	XMFLOAT4 pixelColor;
	bool result;

	stride = sizeof(VertexType);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &sentence->vertexBuffer, &stride, &offset);

	deviceContext->IASetIndexBuffer(sentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	pixelColor = XMFLOAT4(sentence->red, sentence->green, sentence->blue, 1.0f);

	result = m_FontShader->Render(deviceContext, sentence->indexCount, worldMatrix, m_baseViewMatrix, orthoMatrix, m_Font->GetTexture(), pixelColor);
	if (!result) {
		false;
	}

	return true;
}

bool TextClass::SetFps(int fps, ID3D11DeviceContext* deviceContext) {
	char tempString[16];
	char fpsString[16];
	float red, green, blue;
	bool result;

	if (fps > 9999) {
		fps = 9999;
	}
	_itoa_s(fps, tempString, 10);

	strcpy_s(fpsString, "Fps: ");
	strcat_s(fpsString, tempString);

	if (fps >= 60) {
		red = 0.0f;
		green = 1.0f;
		blue = 0.0f;
	}
	if (fps < 60) {
		red = 1.0f;
		green = 1.0f;
		blue = 0.0f;
	}
	if (fps < 30) {
		red = 1.0f;
		green = 0.0f;
		blue = 0.0f;
	}

	result = UpdateSentence(m_sentence1, fpsString, 20, 20, red, green, blue, deviceContext);
	if (!result) {
		return false;
	}

	return true;
}

bool TextClass::SetCpu(int xcpu, ID3D11DeviceContext* deviceContext) {
	char tempString[16];
	char cpuString[16];
	bool result;

	_itoa_s(xcpu, tempString, 10);

	strcpy_s(cpuString, "Cpu: ");
	strcat_s(cpuString, tempString);
	strcat_s(cpuString, "%");

	result = UpdateSentence(m_sentence2, cpuString, 20, 40, 0.0f, 1.0f, 0.0f, deviceContext);
	if (!result) {
		return false;
	}

	return true;
}