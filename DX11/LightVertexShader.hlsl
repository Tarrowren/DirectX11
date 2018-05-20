//���󻺳���
cbuffer MatrixBuffer {
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

//POSITION�����ڶ�����ɫ����SV_POSITION������������ɫ��
//����������������������͹�ķ���֮��ĽǶ����������
struct VertexInputType {
	float4 position:POSITION;
	float2 tex:TEXCOORD0;
	float3 normal:NORMAL;
};

struct PixelInputType {
	float4 position:SV_POSITION;
	float2 tex:TEXCOORD0;
	float3 normal:NORMAL;
};

PixelInputType LightVertexShader(VertexInputType input) {
	PixelInputType output;

	input.position.w = 1.0f;

	//�������硢��ͼ��ͶӰ������㶥���λ��
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	//�洢������ɫ������������
	output.tex = input.tex;

	//��������������㷨����
	output.normal = mul(input.normal, (float3x3)worldMatrix);

	//��׼��������
	output.normal = normalize(output.normal);

	return output;
}