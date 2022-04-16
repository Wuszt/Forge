#include "FullScreenCommon.fxh"
#include "Lighting.fxh"

Texture2D DepthBuffer : register(t0);
cbuffer cbMaterial : register(b1)
{
    float Denominator;
};

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    float2 texCoord = float2((input.UV.x + 1.0f) * 0.5f, 1.0f - (input.UV.y + 1.0f) * 0.5f);
    float depth = CalculateLinearDepth( ProjectionA, ProjectionB, DepthBuffer.Sample(LinearSamplerState, texCoord).r);
    
    depth /= Denominator;
    
    return float4(depth, depth, depth, 1.0f);

}