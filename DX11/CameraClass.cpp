#include"CameraClass.h"

CameraClass::CameraClass() {
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;
}

CameraClass::~CameraClass() {

}

//SetPosition和SetRotation函数用于设置相机的位置和旋转
void CameraClass::SetPosition(float x, float y, float z) {
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;

	return;
}

void CameraClass::SetRotation(float x, float y, float z) {
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;

	return;
}

//GetPosition和GetRotation函数将相机的位置和旋转返回给调用函数
XMFLOAT3 CameraClass::GetPosition() {
	return XMFLOAT3(m_positionX, m_positionY, m_positionZ);
}

XMFLOAT3 CameraClass::GetRotation() {
	return XMFLOAT3(m_rotationX, m_rotationY, m_rotationZ);
}

void CameraClass::Render() {
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookArVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	//设置向上的向量
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	upVector = XMLoadFloat3(&up);

	//设置相机在世界上的位置
	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;

	positionVector = XMLoadFloat3(&position);

	//默认情况下设置摄像头的位置
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	lookArVector = XMLoadFloat3(&lookAt);

	//以弧度设置水平角(X)、俯仰角(Y)和滚动角(Z)旋转
	pitch = m_rotationX * 0.0174532925f;
	yaw = m_rotationY * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;

	//创建旋转矩阵
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	//通过旋转矩阵变换lookAt和up向量，以便视图在原点处正确旋转
	lookArVector = XMVector3TransformCoord(lookArVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	//将旋转的相机位置转换为查看器的位置
	lookArVector = XMVectorAdd(positionVector, lookArVector);

	//最后从三个更新的向量中创建视图矩阵
	m_viewMatrix = XMMatrixLookAtLH(positionVector, lookArVector, upVector);

	return;
}

void CameraClass::GetViewMatrix(XMMATRIX &viewMatrix) {
	viewMatrix = m_viewMatrix;

	return;
}

void CameraClass::RenderBaseViewMatrix() {
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;
	upVector = XMLoadFloat3(&up);

	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;
	positionVector = XMLoadFloat3(&position);

	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;
	lookAtVector = XMLoadFloat3(&lookAt);

	pitch = m_rotationX * 0.0174532925f;
	yaw = m_rotationY * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;

	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	m_baseViewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);

	return;
}

void CameraClass::GetBaseViewMatrix(XMMATRIX& viewMatrix) {
	viewMatrix = m_baseViewMatrix;

	return;
}