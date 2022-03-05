#include "Lighting.fxh"

cbuffer cbDefferedRendering : register(b4)
{
    float4x4 InvVP;
    float4 AmbientLighting;
};

cbuffer cbDefferedLighting : register(b5)
{
    LightData LightingData;
};

Texture2D DiffuseBuffer : register(t0);
Texture2D DepthBuffer : register(t1);
Texture2D NormalsBuffer : register(t2);

float CalculateLinearDepth(float nonLinearDepth)
{
    return ProjectionB / (nonLinearDepth - ProjectionA);
}

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float3 ViewRayWS : TEXCOORD0;
    float2 UV : TEXCOORD1;
};

static const float2 arrBasePos[4] =
{
    float2(-1.0, 1.0),
    float2(-1.0, -1.0),
    float2(1.0, 1.0),
    float2(1.0, -1.0),
};

VS_OUTPUT VS(uint vI : SV_VERTEXID)
{
    VS_OUTPUT output;
    
    output.Position = float4(arrBasePos[vI].xy, 0.0, 1.0);       
    
    float3 positionWS = mul(InvVP, output.Position).xyz;
    output.ViewRayWS = positionWS - CameraPosition;
    
    output.UV = arrBasePos[vI].xy;
    
    return output;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    float2 texCoord = float2((input.UV.x + 1.0f) * 0.5f, 1.0f - (input.UV.y + 1.0f) * 0.5f);
 
    float3 normal = NormalsBuffer.Sample(LinearSamplerState, texCoord).rgb * 2.0f - 1.0f;
    float depth = CalculateLinearDepth(DepthBuffer.Sample(LinearSamplerState, texCoord).r);
    
    float3 viewRay = normalize(input.ViewRayWS);
    float viewZDist = dot(CameraDir, viewRay);
    float3 worldPos = CameraPosition + viewRay * (depth / viewZDist);
    
    return float4( DiffuseBuffer.Sample(LinearSamplerState, texCoord).rgb * LightingData.Color * CalcPointLight(worldPos, normal, LightingData ), 1.0f);

}