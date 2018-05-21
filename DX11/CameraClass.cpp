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

//SetPosition��SetRotation�����������������λ�ú���ת
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

//GetPosition��GetRotation�����������λ�ú���ת���ظ����ú���
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

	//�������ϵ�����
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	upVector = XMLoadFloat3(&up);

	//��������������ϵ�λ��
	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;

	positionVector = XMLoadFloat3(&position);

	//Ĭ���������������ͷ��λ��
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	lookArVector = XMLoadFloat3(&lookAt);

	//�Ի�������ˮƽ��(X)��������(Y)�͹�����(Z)��ת
	pitch = m_rotationX * 0.0174532925f;
	yaw = m_rotationY * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;

	//������ת����
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	//ͨ����ת����任lookAt��up�������Ա���ͼ��ԭ�㴦��ȷ��ת
	lookArVector = XMVector3TransformCoord(lookArVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	//����ת�����λ��ת��Ϊ�鿴����λ��
	lookArVector = XMVectorAdd(positionVector, lookArVector);

	//�����������µ������д�����ͼ����
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