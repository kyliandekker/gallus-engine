struct ModelViewProjection
{
    matrix MVP;
};

ConstantBuffer<ModelViewProjection> ModelViewProjectionCB : register(b0);

struct VertexPosColor
{
    float3 Position : POSITION;
	float3 Normal   : NORMAL;
    float3 Color    : COLOR;
    float2 UV    : TEXCOORD;
};

struct VertexShaderOutput
{
    float4 Position : SV_Position;
	float3 Normal : NORMAL;
	float4 Color    : COLOR;
    float2 UV    : TEXCOORD;
};

VertexShaderOutput main(VertexPosColor IN)
{
    VertexShaderOutput OUT;

    OUT.Position = mul(ModelViewProjectionCB.MVP, float4(IN.Position, 1.0f));
    OUT.Normal = normalize(mul((float3x3)ModelViewProjectionCB.MVP, IN.Normal)); // Transform normal
    OUT.Color = float4(IN.Color, 1.0f);
    OUT.UV = IN.UV;

    return OUT;
}