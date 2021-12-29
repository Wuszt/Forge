cbuffer cbFrame : register(b0)
{
    float time;
};

cbuffer cbMesh : register(b2)
{
    float4x4 W;
};

cbuffer cbCamera : register(b3)
{
    float4x4 VP;
};

SamplerState LinearSampler : register(s0);

float3 CalcAmbient(float3 normal, float3 color)
{
    float3 ambientUp = float3(0.6f, 0.6f, 0.6f);
    float3 ambientDown = float3(0.15f, 0.15f, 0.15f);
    float a = (1.0f + normal.z) * 0.5f;
    float3 ambient = ambientUp * a + (1.0f - a) * ambientDown;
    return ambient * color;
}