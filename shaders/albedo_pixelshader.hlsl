Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

struct PixelShaderInput
{
    float4 Position : SV_Position;
	float3 Normal   : NORMAL;
    float4 Color    : COLOR;
    float2 UV       : TEXCOORD;
};

struct Material
{
    float3 DiffuseColor;
    float Metallic;
    float Smoothness;
};

ConstantBuffer<Material> MaterialCB : register(b1);

struct DirectionalLight
{
    float3 LightDirection;
    float  Padding;
    float3 LightColor;
    float  AmbientIntensity;
};

ConstantBuffer<DirectionalLight> DirectionalLightCB : register(b2);

float4 main(PixelShaderInput IN) : SV_Target
{
    // Light Direction and Normalization
    float3 normal = normalize(IN.Normal);
    float3 lightDir = normalize(-DirectionalLightCB.LightDirection);  // Assuming directional light

    // Diffuse Lighting (Lambertian reflection)
    float diff = max(dot(normal, lightDir), 0.0f);
    float3 diffuse = diff * DirectionalLightCB.LightColor * MaterialCB.DiffuseColor;

    // Ambient Lighting
    float3 ambient = DirectionalLightCB.AmbientIntensity * DirectionalLightCB.LightColor * MaterialCB.DiffuseColor;

    // Combine Lighting
    float3 lighting = ambient + diffuse;

    // Sample Diffuse Texture
    float4 texColor = texture0.Sample(sampler0, IN.UV);
    float3 finalColor = texColor.rgb * lighting;

    return float4(finalColor, texColor.a);  // Return the final color with the texture alpha
}