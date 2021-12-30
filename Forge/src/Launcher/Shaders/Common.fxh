cbuffer cbFrame : register(b0)
{
    float Time;
};

cbuffer cbMesh : register(b2)
{
    float4x4 W;
};

cbuffer cbCamera : register(b3)
{
    float4x4 VP;
    float3 CameraPosition;
    float pad;
};

SamplerState LinearSampler : register(s0);

float3 CalcAmbient(float3 normal)
{
    float3 ambientUp = float3(0.2f, 0.2f, 0.2f);
    float3 ambientDown = float3(0.15f, 0.15f, 0.15f);
    float a = (1.0f + normal.z) * 0.5f;
    return ambientUp * a + (1.0f - a) * ambientDown;
}

float3 CalculateBlinnPhong( float3 toLight, float3 position, float3 normal )
{
    float3 diffuseColor = float3(1.0f, 1.0f, 1.0f);
    float3 specularColor = float3(1.0f, 1.0f, 1.0f);
    
    
    float3 toCamera = normalize(CameraPosition - position);
    
    float3 H = normalize(toLight + toCamera);
    
    float diffuse = saturate(dot(toLight, normal));
    float specular = saturate(dot(H, normal));
    specular = pow(specular, 16.0f);
    return diffuseColor * diffuse + specularColor * specular;
}

float3 CalcDirectionalLight( float3 position, float3 normal )
{
    float3 toLight = normalize(float3(1.0f, 1.0f, 1.0f));
    return CalculateBlinnPhong(toLight, position, normal);
}

float3 CalcPointLight( float3 position, float3 normal )
{
    float3 lightPosition = float3(sin(Time) * 500.0f, sin(Time) * 500.0f, 250.0f);
    float lightRange = 1000.0f;
    float3 toLight = lightPosition - position;
    float distToLight = length(toLight);
    toLight /= distToLight;
    
    float distToLightNorm = 1.0f - saturate(distToLight / lightRange);
    float attn = distToLightNorm * distToLightNorm;
    
    return CalculateBlinnPhong(toLight, position, normal) * attn;

}