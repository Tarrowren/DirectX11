//矩阵缓冲区
cbuffer MatrixBuffer {
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

//POSITION适用于顶点着色器，SV_POSITION适用于像素着色器
//法线向量是利用正常方向和光的方向之间的角度来计算光量
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

	//根据世界、视图和投影矩阵计算顶点的位置
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	//存储像素着色器的纹理坐标
	output.tex = input.tex;

	//仅针对世界矩阵计算法向量
	output.normal = mul(input.normal, (float3x3)worldMatrix);

	//标准化法向量
	output.normal = normalize(output.normal);

	return output;
}