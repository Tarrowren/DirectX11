//���󻺳���
cbuffer MatrixBuffer {
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

//POSITION�����ڶ�����ɫ����SV_POSITION������������ɫ����COLOR����������
struct VertexInputType {
	float4 position:POSITION;
	float4 color:COLOR;
};

struct PixelInputType {
	float4 position:SV_POSITION;
	float4 color:COLOR;
};

PixelInputType ColorVertexShader(VertexInputType input) {
	PixelInputType output;

	input.position.w = 1.0f;

	//�������硢��ͼ��ͶӰ������㶥���λ�á�
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	//�洢Ҫʹ�õ�������ɫ����������ɫ��
	output.color = input.color;

	return output;
}