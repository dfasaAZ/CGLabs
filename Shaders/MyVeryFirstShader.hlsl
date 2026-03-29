cbuffer ConstantBuffer : register(b0)
{
    float4x4 worldViewProj;
    float4x4 world;
};

struct VS_IN
{
    float4 pos : POSITION;
    float4 col : COLOR;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 col : COLOR;
};

PS_IN VSMain(VS_IN input)
{
    PS_IN output;
    output.pos = mul(input.pos, worldViewProj);
    output.col = input.col;
    return output;
}

float4 PSMain(PS_IN input) : SV_Target
{
    float4 col = input.col;
#ifdef TEST
    if (input.pos.x > 400) col = TCOLOR;
#endif
    return col;
}