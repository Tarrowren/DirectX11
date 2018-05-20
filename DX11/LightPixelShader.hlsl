Texture2D shaderTexture;
SamplerState SampleType;

cbuffer LightBuffer {
	float4 diffuseColor;//漫反射颜色
	float3 lightDirection;//光的方向
	float padding;
};

struct PixelInputType {
	float4 position:SV_POSITION;
	float2 tex:TEXCOORD0;
	float3 normal:NORMAL;
};

float4 LightPixelShader(PixelInputType input) :SV_TARGET{
	float4 textureColor;
float3 lightDir;
float lightIntensity;//光强
float4 color;

//使用此纹理坐标位置处的采样器从纹理中采样像素颜色
textureColor = shaderTexture.Sample(SampleType, input.tex);

//反转光线方向进行计算
lightDir = -lightDirection;

//计算此像素上的光强
lightIntensity = saturate(dot(input.normal, lightDir));

//根据漫反射颜色与光强度的组合确定最终的漫反射颜色
color = saturate(diffuseColor* lightIntensity);

//将纹理像素和最终漫反射颜色相乘以得到最终的像素颜色结果
color = color * textureColor;

return color;
}