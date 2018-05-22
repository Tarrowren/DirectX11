#pragma once

#include"TextClass.h"

class UserInterfaceClass {

public:

	UserInterfaceClass();
	~UserInterfaceClass();

	bool Initialize(D3DClass*, int, int);
	void Shutdown();
	bool Frame(ID3D11DeviceContext*, int, int, float, float, float, float, float, float);
	bool Render(D3DClass*, ShaderManagerClass*, XMMATRIX, XMMATRIX, XMMATRIX);

private:

	bool UpdateFpsString(ID3D11DeviceContext*, int);
	bool UpdateCpuString(ID3D11DeviceContext*, int);
	bool UpdatePositionStrings(ID3D11DeviceContext*, float, float, float, float, float, float);

private:

	FontClass * m_Font1;
	TextClass *m_FpsString, *m_CpuString, *m_VideoStrings, *m_PositionStrings;
	int m_previousFps;
	int m_previousCpu;
	int m_previousPosition[6];

};