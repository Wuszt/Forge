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