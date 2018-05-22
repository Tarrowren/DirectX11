#include"ZoneClass.h"

ZoneClass::ZoneClass() {
	m_UserInterface = 0;
	m_Camera = 0;
	m_Position = 0;
	m_Terrain = 0;
}

ZoneClass::~ZoneClass() {

}

bool ZoneClass::Initialize(D3DClass* Direct3D, HWND hwnd, int screenWidth, int screenHeight, float screenDepth) {
	bool result;

	m_UserInterface = new UserInterfaceClass;
	if (!m_UserInterface) {
		return false;
	}
	result = m_UserInterface->Initialize(Direct3D, screenHeight, screenWidth);
	if (!result) {
		MessageBox(hwnd, L"无法初始化用户界面对象。", L"错误", MB_OK);
		return false;
	}

	m_Camera = new CameraClass;
	if (!m_Camera) {
		return false;
	}
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	m_Camera->Render();
	m_Camera->RenderBaseViewMatrix();

	m_Position = new PositionClass;
	if (!m_Position) {
		return false;
	}
	m_Position->SetPosition(128.0f, 5.0f, -10.0f);
	m_Position->SetRotation(0.0f, 0.0f, 0.0f);

	m_Terrain = new TerrainClass;
	if (!m_Terrain) {
		return false;
	}
	result = m_Terrain->Initialize(Direct3D->GetDevice());
	if (!result) {
		MessageBox(hwnd, L"无法初始化地形对象。", L"错误", MB_OK);
		return false;
	}

	m_displayUI = true;

	return true;
}

void ZoneClass::Shutdown() {
	if (m_Terrain) {
		m_Terrain->Shutdown();
		delete m_Terrain;
		m_Terrain = 0;
	}
	if (m_Position) {
		delete m_Position;
		m_Position = 0;
	}
	if (m_Camera) {
		delete m_Camera;
		m_Camera = 0;
	}
	if (m_UserInterface) {
		m_UserInterface->Shutdown();
		delete m_UserInterface;
		m_UserInterface = 0;
	}

	return;
}

bool ZoneClass::Frame(D3DClass* Direct3D, InputClass* Input, ShaderManagerClass* ShaderManager, float frameTime, int fps) {
	bool result;
	float posX, posY, posZ, rotX, rotY, rotZ;

	HandleMovementInput(Input, frameTime);

	m_Position->GetPosition(posX, posY, posZ);
	m_Position->GetRotation(rotX, rotY, rotZ);

	result = m_UserInterface->Frame(Direct3D->GetDeviceContext(), fps, posX, posY, posZ, rotX, rotY, rotZ);
	if (!result) {
		return false;
	}

	result = Render(Direct3D, ShaderManager);
	if (!result) {
		return false;
	}

	return true;
}

void ZoneClass::HandleMovementInput(InputClass* Input, float frameTime) {
	bool keyDown;
	float posX, posY, posZ, rotX, rotY, rotZ;

	m_Position->SetFrameTime(frameTime);

	//左看
	keyDown = Input->IsLeftPressed();
	m_Position->TurnLeft(keyDown);
	//右看
	keyDown = Input->IsRightPressed();
	m_Position->TurnRight(keyDown);
	//上看
	keyDown = Input->IsUpPressed();
	m_Position->LookUpward(keyDown);
	//下看
	keyDown = Input->IsDownPressed();
	m_Position->LookDownward(keyDown);
	//靠近
	keyDown = Input->IsPgUpPressed();
	m_Position->MoveViewForward(keyDown);
	//远离
	keyDown = Input->IsPgDownPressed();
	m_Position->MoveViewBackward(keyDown);

	//上升
	keyDown = Input->IsSpacePressed();
	m_Position->MoveUpward(keyDown);
	//下降
	keyDown = Input->IsCPressed();
	m_Position->MoveDownward(keyDown);
	//前进
	keyDown = Input->IsWPressed();
	m_Position->MoveForward(keyDown);
	//后退
	keyDown = Input->IsSPressed();
	m_Position->MoveBackward(keyDown);
	//左
	keyDown = Input->IsAPressed();
	m_Position->MoveLeftward(keyDown);
	//右
	keyDown = Input->IsDPressed();
	m_Position->MoveRightward(keyDown);

	//环视
	int mouseChangeX, mouseChangeY;
	Input->GetMouseChange(mouseChangeX, mouseChangeY);
	m_Position->MouseLookAround(mouseChangeX, mouseChangeY);
	//缩放
	int mouseChangeZ;
	Input->GetMouseWheelChange(mouseChangeZ);
	m_Position->MouseScaling(mouseChangeZ);

	m_Position->GetPosition(posX, posY, posZ);
	m_Position->GetRotation(rotX, rotY, rotZ);

	m_Camera->SetPosition(posX, posY, posZ);
	m_Camera->SetRotation(rotX, rotY, rotZ);

	if (Input->IsF1Toggled()) {
		m_displayUI = !m_displayUI;
	}

	return;
}

bool ZoneClass::Render(D3DClass* Direct3D, ShaderManagerClass* ShaderManager) {
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, baseViewMatrix, orthoMatrix;
	bool result;

	m_Camera->Render();

	Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Camera->GetBaseViewMatrix(baseViewMatrix);
	Direct3D->GetOrthoMatrix(orthoMatrix);

	Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	m_Terrain->Render(Direct3D->GetDeviceContext());

	result = ShaderManager->RenderColorShader(Direct3D->GetDeviceContext(), m_Terrain->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	if (!result) {
		return false;
	}

	if (m_displayUI) {
		result = m_UserInterface->Render(Direct3D, ShaderManager, worldMatrix, baseViewMatrix, orthoMatrix);
		if (!result) {
			return false;
		}
	}

	Direct3D->EndScene();

	return true;
}