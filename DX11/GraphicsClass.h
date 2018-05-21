#pragma once
#include"D3DClass.h"
#include"CameraClass.h"
#include"ModelClass.h"
#include"LightShaderClass.h"
#include"LightClass.h"
#include"TextClass.h"

const bool FULL_SCREEN = false;//全屏
const bool VSYNC_ENABLED = true;//垂直同步
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class GraphicsClass {

public:

	GraphicsClass();
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame(int, int, float, int, int);
	bool Render();

private:

	D3DClass * m_Direct3D;
	CameraClass* m_Camera;
	ModelClass* m_Model;
	LightShaderClass* m_LightShader;
	LightClass* m_Light;
	TextClass* m_Text;

};