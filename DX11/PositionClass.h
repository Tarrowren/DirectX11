#pragma once
#include<math.h>

class PositionClass{

public:

	PositionClass();
	~PositionClass();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);
	void GetPosition(float&, float&, float&);
	void GetRotation(float&, float&, float&);

	//SetFrameTime用于使相机与应用程序的速度保持同步
	void SetFrameTime(float);

	//根据用户键盘输入来移动相机
	void MoveForward(bool);
	void MoveBackward(bool);
	void MoveLeftward(bool);
	void MoveRightward(bool);
	void MoveUpward(bool);
	void MoveDownward(bool);

	void MoveViewForward(bool);
	void MoveViewBackward(bool);
	void TurnLeft(bool);
	void TurnRight(bool);
	void LookUpward(bool);
	void LookDownward(bool);

	//根据用户鼠标输入来转动相机
	void MouseLookAround(int, int);
	void MouseScaling(int);

private:

	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;

	float m_frameTime;

	float m_forwardSpeed, m_backwardSpeed;
	float m_leftwardSpeed, m_rightwardSpeed;
	float m_upwardSpeed, m_downwardSpeed;

	float m_viewforwardSpeed, m_viewbackwardSpeed;
	float m_leftTurnSpeed, m_rightTurnSpeed;
	float m_lookUpSpeed, m_lookDownSpeed;

};