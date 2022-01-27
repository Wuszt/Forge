#include "FullScreenCommon.fxh"
#include "Lighting.fxh"

Texture2D DiffuseBuffer : register(t0);
Texture2D DepthBuffer : register(t1);
Texture2D NormalsBuffer : register(t2);

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    float3 viewRay = normalize(float3(input.Position.xy / input.Position.z, 1.0f));
    
    float2 texCoord = float2((input.UV.x + 1.0f) * 0.5f, 1.0f - (input.UV.y + 1.0f) * 0.5f);
 
    float3 diffuse = DiffuseBuffer.Sample(LinearSamplerState, texCoord).rgb;
    float3 normal = NormalsBuffer.Sample(LinearSamplerState, texCoord).rgb;
    float depth = CalculateLinearDepth(DepthBuffer.Sample(LinearSamplerState, texCoord).r);
    
    float viewZDist = dot(CameraDir, viewRay);
    float3 worldPos = CameraPosition + viewRay * (depth / viewZDist);
    
    return float4( diffuse * CalculateLighting(worldPos, normal), 1.0f);
}