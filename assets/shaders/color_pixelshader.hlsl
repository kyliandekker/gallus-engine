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

float4 main( PixelShaderInput IN ) : SV_Target
{
    // Normalize input normal (assumes interpolated normals are not unit-length)
    float3 N = normalize(IN.Normal);
    float3 L = normalize(-DirectionalLightCB.LightDirection); // Light direction (negated)
    float3 V = normalize(-IN.Position.xyz); // View direction (assumes camera at (0,0,0))

    // Diffuse shading (Lambertian)
    float NdotL = max(dot(N, L), 0);
    float3 diffuse = MaterialCB.DiffuseColor * DirectionalLightCB.LightColor * NdotL;

    // Specular reflection (Cook-Torrance GGX Approximation)
    float3 H = normalize(L + V); // Halfway vector
    float NdotH = max(dot(N, H), 0);
    float roughness = 1.0 - MaterialCB.Smoothness; // Convert Smoothness to roughness
    float specularIntensity = pow(NdotH, 2 / (roughness * roughness + 0.001));

    // Fresnel-Schlick Approximation
    float3 F0 = lerp(float3(0.04, 0.04, 0.04), MaterialCB.DiffuseColor, MaterialCB.Metallic); // Base reflectance
    float3 fresnel = F0 + (1 - F0) * pow(1 - max(dot(V, H), 0), 5);

    // Final Specular component
    float3 specular = fresnel * specularIntensity;

    // Combine diffuse and specular based on metallic
    float3 finalColor = (1 - MaterialCB.Metallic) * diffuse + specular;

    // Add ambient lighting
    finalColor += MaterialCB.DiffuseColor * DirectionalLightCB.AmbientIntensity;

    return float4(finalColor, 1);
}