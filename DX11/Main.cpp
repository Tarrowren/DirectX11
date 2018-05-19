#include"SystemClass.h"
#include"D3DClass.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow) {
	SystemClass *System;
	bool result;

	//����ϵͳ����
	System = new SystemClass;
	if (!System) {
		return 0;
	}

	//��ʼ��������ϵͳ����
	result = System->Initialize();
	if (result) {
		System->Run();
	}

	//�رղ��ͷ�ϵͳ����
	System->Shutdown();
	delete System;
	System = 0;

	return 0;
}