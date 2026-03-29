cbuffer TransformBuffer : register(b0)
{
	float4x4 worldViewProj;
	float4x4 world;
};

struct VS_INPUT
{
	float4 position : POSITION;
	float4 color : COLOR;
};

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

PS_INPUT VSMain(VS_INPUT input)
{
	PS_INPUT output;
	output.position = mul(input.position, worldViewProj);
	output.color = input.color;
	return output;
}

float4 PSMain(PS_INPUT input) : SV_TARGET
{
	
	return input.color;
}