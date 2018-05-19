#include"SystemClass.h"
#include"D3DClass.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow) {
	SystemClass *System;
	bool result;

	//创建系统对象
	System = new SystemClass;
	if (!System) {
		return 0;
	}

	//初始化并运行系统对象
	result = System->Initialize();
	if (result) {
		System->Run();
	}

	//关闭并释放系统对象
	System->Shutdown();
	delete System;
	System = 0;

	return 0;
}