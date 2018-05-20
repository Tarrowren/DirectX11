Texture2D shaderTexture;
SamplerState SampleType;

cbuffer LightBuffer {
	float4 diffuseColor;//��������ɫ
	float3 lightDirection;//��ķ���
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
float lightIntensity;//��ǿ
float4 color;

//ʹ�ô���������λ�ô��Ĳ������������в���������ɫ
textureColor = shaderTexture.Sample(SampleType, input.tex);

//��ת���߷�����м���
lightDir = -lightDirection;

//����������ϵĹ�ǿ
lightIntensity = saturate(dot(input.normal, lightDir));

//������������ɫ���ǿ�ȵ����ȷ�����յ���������ɫ
color = saturate(diffuseColor* lightIntensity);

//���������غ�������������ɫ����Եõ����յ�������ɫ���
color = color * textureColor;

return color;
}