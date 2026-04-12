cbuffer ConstantBuffer : register(b0)
{
    float4x4 worldViewProj;
    float4x4 world;
    float3 lightDirection;
    float padding;
};

struct VSInput
{
    float3 position : POSITION;
    float4 color : COLOR;
    float3 normal : NORMAL;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float3 normal : NORMAL;
    float3 worldPos : POSITION;
};

PSInput VSMain(VSInput input)
{
    PSInput output;
    
    float4 worldPos = mul(float4(input.position, 1.0f), world);
    output.position = mul(worldPos, worldViewProj);
    output.worldPos = worldPos.xyz;
    
    // Transform normal to world space
    output.normal = mul(input.normal, (float3x3) world);
    
    // Simple directional lighting
    float3 lightDir = normalize(float3(1.0f, 1.0f, 1.0f));
    float3 normal = normalize(output.normal);
    
    float ambientStrength = 0.3f;
    float3 ambient = ambientStrength;
    
    float diff = max(dot(normal, lightDir), 0.0f);
    float3 diffuse = diff;
    
    float3 lighting = ambient + diffuse;
    output.color = float4(input.color.rgb * lighting, input.color.a);
    
    return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return input.color;
}