#include"PositionClass.h"

PositionClass::PositionClass() {
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;
	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;

	m_frameTime = 0.0f;

	m_forwardSpeed = 0.0f;
	m_backwardSpeed = 0.0f;
	m_leftwardSpeed = 0.0f;
	m_rightwardSpeed = 0.0f;
	m_upwardSpeed = 0.0f;
	m_downwardSpeed = 0.0f;
	
	m_viewforwardSpeed = 0.0f;
	m_viewbackwardSpeed = 0.0f;
	m_leftTurnSpeed = 0.0f;
	m_rightTurnSpeed = 0.0f;
	m_lookUpSpeed = 0.0f;
	m_lookDownSpeed = 0.0f;
}

PositionClass::~PositionClass() {

}

void PositionClass::SetPosition(float x, float y, float z) {
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;

	return;
}

void PositionClass::SetRotation(float x, float y, float z) {
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;

	return;
}

void PositionClass::GetPosition(float& x, float& y, float& z) {
	x = m_positionX;
	y = m_positionY;
	z = m_positionZ;

	return;
}

void PositionClass::GetRotation(float& x, float& y, float& z) {
	x = m_rotationX;
	y = m_rotationY;
	z = m_rotationZ;

	return;
}

void PositionClass::SetFrameTime(float time) {
	m_frameTime = time;

	return;
}


void PositionClass::MoveForward(bool keydown) {
	float radiansY;

	//根据帧时间和用户是否按住按键来更新前进速度
	if (keydown) {
		m_forwardSpeed += m_frameTime * 1.0f;
		if (m_forwardSpeed > (m_frameTime * 50.0f)) {
			m_forwardSpeed = m_frameTime * 50.0f;
		}
	}
	else {
		m_forwardSpeed -= m_frameTime * 0.5f;
		if (m_forwardSpeed < 0.0f) {
			m_forwardSpeed = 0.0f;
		}
	}

	//将度数转换为弧度
	radiansY = m_rotationY * 0.0174532925f;

	//更新位置
	m_positionX += sinf(radiansY) * m_forwardSpeed;
	m_positionZ += cosf(radiansY) * m_forwardSpeed;

	return;
}

void PositionClass::MoveBackward(bool keydown) {
	float radiansY;

	if (keydown) {
		m_backwardSpeed += m_frameTime * 1.0f;
		if (m_backwardSpeed > (m_frameTime * 50.0f)) {
			m_backwardSpeed = m_frameTime * 50.0f;
		}
	}
	else {
		m_backwardSpeed -= m_frameTime * 0.5f;
		if (m_backwardSpeed < 0.0f) {
			m_backwardSpeed = 0.0f;
		}
	}

	radiansY = m_rotationY * 0.0174532925f;

	m_positionX -= sinf(radiansY) * m_backwardSpeed;
	m_positionZ -= cosf(radiansY) * m_backwardSpeed;

	return;
}

void PositionClass::MoveLeftward(bool keydown) {
	float radiansY;

	if (keydown) {
		m_leftwardSpeed += m_frameTime * 1.0f;
		if (m_leftwardSpeed > (m_frameTime * 50.0f)) {
			m_leftwardSpeed = m_frameTime * 50.0f;
		}
	}
	else {
		m_leftwardSpeed -= m_frameTime * 0.5f;
		if (m_leftwardSpeed < 0.0f) {
			m_leftwardSpeed = 0.0f;
		}
	}

	radiansY = m_rotationY * 0.0174532925f;

	m_positionX -= cosf(radiansY) * m_leftwardSpeed;
	m_positionZ += sinf(radiansY) * m_leftwardSpeed;

	return;
}

void PositionClass::MoveRightward(bool keydown) {
	float radiansY;

	if (keydown) {
		m_rightwardSpeed += m_frameTime * 1.0f;
		if (m_rightwardSpeed > (m_frameTime * 50.0f)) {
			m_rightwardSpeed = m_frameTime * 50.0f;
		}
	}
	else {
		m_rightwardSpeed -= m_frameTime * 0.5f;
		if (m_rightwardSpeed < 0.0f) {
			m_rightwardSpeed = 0.0f;
		}
	}

	radiansY = m_rotationY * 0.0174532925f;

	m_positionX += cosf(radiansY) * m_rightwardSpeed;
	m_positionZ -= sinf(radiansY) * m_rightwardSpeed;

	return;
}

void PositionClass::MoveUpward(bool keydown) {
	if (keydown) {
		m_upwardSpeed += m_frameTime * 1.5f;
		if (m_upwardSpeed > (m_frameTime * 15.0f)) {
			m_upwardSpeed = m_frameTime * 15.0f;
		}
	}
	else {
		m_upwardSpeed -= m_frameTime * 0.5f;
		if (m_upwardSpeed < 0.0f) {
			m_upwardSpeed = 0.0f;
		}
	}

	m_positionY += m_upwardSpeed;

	return;
}

void PositionClass::MoveDownward(bool keydown) {
	if (keydown) {
		m_downwardSpeed += m_frameTime * 1.5f;
		if (m_downwardSpeed > (m_frameTime * 15.0f)) {
			m_downwardSpeed = m_frameTime * 15.0f;
		}
	}
	else {
		m_downwardSpeed -= m_frameTime * 0.5f;
		if (m_downwardSpeed < 0.0f) {
			m_downwardSpeed = 0.0f;
		}
	}

	m_positionY -= m_downwardSpeed;

	return;
}


void PositionClass::MoveViewForward(bool keydown) {
	float radiansX, radiansY;

	if (keydown) {
		m_viewforwardSpeed += m_frameTime * 1.0f;
		if (m_viewforwardSpeed > (m_frameTime * 50.0f)) {
			m_viewforwardSpeed = m_frameTime * 50.0f;
		}
	}
	else {
		m_viewforwardSpeed -= m_frameTime * 0.5f;
		if (m_viewforwardSpeed < 0.0f) {
			m_viewforwardSpeed = 0.0f;
		}
	}

	radiansX = m_rotationX * 0.0174532925f;
	radiansY = m_rotationY * 0.0174532925f;

	m_positionY -= sinf(radiansX) * m_viewforwardSpeed;
	m_positionX += sinf(radiansY) * cosf(radiansX) * m_viewforwardSpeed;
	m_positionZ += cosf(radiansY) * cosf(radiansX) * m_viewforwardSpeed;

	return;
}

void PositionClass::MoveViewBackward(bool keydown) {
	float radiansX, radiansY;

	if (keydown) {
		m_viewbackwardSpeed += m_frameTime * 1.0f;
		if (m_viewbackwardSpeed > (m_frameTime * 50.0f)) {
			m_viewbackwardSpeed = m_frameTime * 50.0f;
		}
	}
	else {
		m_viewbackwardSpeed -= m_frameTime * 0.5f;
		if (m_viewbackwardSpeed < 0.0f) {
			m_viewbackwardSpeed = 0.0f;
		}
	}

	radiansX = m_rotationX * 0.0174532925f;
	radiansY = m_rotationY * 0.0174532925f;

	m_positionY += sinf(radiansX) * m_viewbackwardSpeed;
	m_positionX -= sinf(radiansY) * cosf(radiansX) * m_viewbackwardSpeed;
	m_positionZ -= cosf(radiansY) * cosf(radiansX) * m_viewbackwardSpeed;

	return;
}

void PositionClass::TurnLeft(bool keydown) {
	if (keydown) {
		m_leftTurnSpeed += m_frameTime * 5.0f;
		if (m_leftTurnSpeed > (m_frameTime * 150.0f)) {
			m_leftTurnSpeed = m_frameTime * 150.0f;
		}
	}
	else {
		m_leftTurnSpeed -= m_frameTime * 3.5f;
		if (m_leftTurnSpeed < 0.0f) {
			m_leftTurnSpeed = 0.0f;
		}
	}

	m_rotationY -= m_leftTurnSpeed;
	if (m_rotationY < 0.0f) {
		m_rotationY += 360.0f;
	}

	return;
}

void PositionClass::TurnRight(bool keydown) {
	if (keydown) {
		m_rightTurnSpeed += m_frameTime * 5.0f;
		if (m_rightTurnSpeed >(m_frameTime * 150.0f)) {
			m_rightTurnSpeed = m_frameTime * 150.0f;
		}
	}
	else {
		m_rightTurnSpeed -= m_frameTime * 3.5f;
		if (m_rightTurnSpeed < 0.0f) {
			m_rightTurnSpeed = 0.0f;
		}
	}

	m_rotationY += m_rightTurnSpeed;

	if (m_rotationY > 360.0f) {
		m_rotationY -= 360.0f;
	}

	return;
}

void PositionClass::LookUpward(bool keydown) {
	if (keydown) {
		m_lookUpSpeed += m_frameTime * 7.5f;
		if (m_lookUpSpeed > (m_frameTime * 75.0f)) {
			m_lookUpSpeed = m_frameTime * 75.0f;
		}
	}
	else {
		m_lookUpSpeed -= m_frameTime * 2.0f;
		if (m_lookUpSpeed < 0.0f) {
			m_lookUpSpeed = 0.0f;
		}
	}

	m_rotationX -= m_lookUpSpeed;

	if (m_rotationX < -90.0f) {
		m_rotationX = -90.0f;
	}

	return;
}

void PositionClass::LookDownward(bool keydown) {
	if (keydown) {
		m_lookDownSpeed += m_frameTime * 7.5f;
		if (m_lookDownSpeed > (m_frameTime * 75.0f)) {
			m_lookDownSpeed = m_frameTime * 75.0f;
		}
	}
	else {
		m_lookDownSpeed -= m_frameTime * 2.0f;
		if (m_lookDownSpeed < 0.0f) {
			m_lookDownSpeed = 0.0f;
		}
	}

	m_rotationX += m_lookDownSpeed;

	if (m_rotationX > 90.0f) {
		m_rotationX = 90.0f;
	}

	return;
}

void PositionClass::MouseLookAround(int mouseChangeX, int mouseChangeY) {
	int s = 15;
	m_rotationY += (float)mouseChangeX * s / 100;
	m_rotationX += (float)mouseChangeY * s / 100;

	if (m_rotationX > 90.0f) {
		m_rotationX = 90.0f;
	}
	if (m_rotationX < -90.0f) {
		m_rotationX = -90.0f;
	}
	if (m_rotationY < 0.0f) {
		m_rotationY += 360.0f;
	}
	if (m_rotationY > 360.0f) {
		m_rotationY -= 360.0f;
	}

	return;
}