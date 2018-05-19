//矩阵缓冲区
cbuffer MatrixBuffer {
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

//POSITION适用于顶点着色器，SV_POSITION适用于像素着色器，COLOR适用于两者
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

	//根据世界、视图和投影矩阵计算顶点的位置。
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	//存储要使用的像素着色器的输入颜色。
	output.color = input.color;

	return output;
}