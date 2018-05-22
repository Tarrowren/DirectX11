#pragma once

#include"D3DClass.h"
#include"InputClass.h"
#include"ShaderManagerClass.h"
#include"TimerClass.h"
#include"UserInterfaceClass.h"
#include"CameraClass.h"
#include"PositionClass.h"
#include"TerrainClass.h"

class ZoneClass {

public:

	ZoneClass();
	~ZoneClass();

	bool Initialize(D3DClass*, HWND, int, int, float);
	void Shutdown();
	bool Frame(D3DClass*, InputClass*, ShaderManagerClass*, float, int, int);

private:

	void HandleMovementInput(InputClass*, float);
	bool Render(D3DClass*, ShaderManagerClass*);

private:

	UserInterfaceClass * m_UserInterface;
	CameraClass* m_Camera;
	PositionClass* m_Position;
	TerrainClass* m_Terrain;
	bool m_displayUI, m_wireFrame;

};