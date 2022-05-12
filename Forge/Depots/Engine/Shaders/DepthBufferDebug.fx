#include "FullScreenCommon.fxh"
#include "Lighting.fxh"

#ifdef __DEPTH_CUBE_INDEX__
TextureCube DepthBuffer : register(t0);
#else
Texture2D DepthBuffer : register(t0);
#endif

cbuffer cbMaterial : register(b1)
{
    float Denominator;
};

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    float2 texCoord = float2((input.UV.x + 1.0f) * 0.5f, 1.0f - (input.UV.y + 1.0f) * 0.5f);
    float len = length(texCoord);
    float thirdCoord = sqrt(1.0f - len * len);
    
#ifdef __DEPTH_CUBE_INDEX__
    
    texCoord = input.UV;
    
    float3 coords[] =
    {
    float3( texCoord.x, texCoord.y, thirdCoord ),
    float3( -texCoord.x, texCoord.y, -thirdCoord ),
    float3( thirdCoord, texCoord.y, texCoord.x ),
    float3( -thirdCoord, texCoord.y, -texCoord.x ),
    float3( texCoord.x, thirdCoord, texCoord.y ),
    float3( texCoord.x, -thirdCoord, texCoord.y )
};
    
    float depth = CalculateLinearDepth(ProjectionA, ProjectionB, DepthBuffer.Sample(LinearSamplerState, coords[__DEPTH_CUBE_INDEX__]).r);
#else
    float depth = CalculateLinearDepth(ProjectionA, ProjectionB, DepthBuffer.Sample(LinearSamplerState, texCoord).r);
#endif
    depth /= Denominator;
    
    return float4(depth, depth, depth, 1.0f);

}