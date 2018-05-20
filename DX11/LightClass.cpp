#include"LightClass.h"

LightClass::LightClass() {

}

LightClass::~LightClass() {

}

void LightClass::SetDiffuseColor(float red, float green, float blue, float alpht) {
	m_diffuseColor = XMFLOAT4(red, green, blue, alpht);

	return;
}

void LightClass::SetDirection(float x, float y, float z) {
	m_direction = XMFLOAT3(x, y, z);

	return;
}

XMFLOAT4 LightClass::GetDiffuseColor() {
	return m_diffuseColor;
}

XMFLOAT3 LightClass::GetDirection() {
	return m_direction;
}