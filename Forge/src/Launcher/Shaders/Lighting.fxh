#ifndef __LIGHTING_HEADER__
#define __LIGHTING_HEADER__

#include "Common.fxh"

struct PointLightData
{
    float3 Position;
    float Power;
    float3 Color;
    float ProjectionA;
    float ProjectionB;
};

struct SpotLightData
{
    float3 Position;
    float InnerAngle;
    float3 Direction;
    float OuterAngle;  
    float3 Color;
    float Power;
    float4x4 VP;
};

struct DirectionalLightData
{
    float3 Direction;
    float padding0;
    float3 Color;
    float padding1;
    float4x4 VP;
};

#ifdef __POINT_LIGHT__
typedef PointLightData LightData;
#elif defined __SPOT_LIGHT__
typedef SpotLightData LightData;
#elif defined __DIRECTIONAL_LIGHT__
typedef DirectionalLightData LightData;
#endif

float3 CalcAmbient(float3 surfNormal, float3 ambientUp, float3 ambientDown)
{
    float a = (1.0f + surfNormal.z) * 0.5f;
    return ambientUp * a + (1.0f - a) * ambientDown;
}

float3 CalculateBlinnPhong(float3 toLight, float3 surfPos, float3 surfNormal)
{
    float3 diffuseColor = float3(1.0f, 1.0f, 1.0f);
    float3 specularColor = float3(1.0f, 1.0f, 1.0f);
      
    float3 toCamera = normalize(CameraPosition - surfPos);
    
    float3 H = normalize(toLight + toCamera);
    
    float diffuse = saturate(dot(toLight, surfNormal));
    float specular = saturate(dot(H, surfNormal));
    specular = pow(specular, 16.0f);
    specular = 0.0f;
    return saturate(diffuseColor * diffuse + specularColor * specular);
}

float3 CalcDirectionalLight(float3 surfPos, float3 surfNormal, DirectionalLightData lightingData)
{
    return CalculateBlinnPhong( -lightingData.Direction, surfPos, surfNormal);
}

float3 CalcPointLight(float3 surfPos, float3 surfNormal, PointLightData lightingData)
{
    const float3 lightPosition = lightingData.Position;
    const float3 att = float3(0.01f, 0.01f, 0.01f);
    
    float3 toLight = lightPosition - surfPos;
    float distToLight = length(toLight);
    toLight /= distToLight;
    
    float3 mult = float3(1.0f, distToLight, distToLight * distToLight);
    
    float attn = saturate(lightingData.Power / dot(att, mult));
    return CalculateBlinnPhong(toLight, surfPos, surfNormal) * attn;
}

float3 CalcSpotLight(float3 surfPos, float3 surfNormal, SpotLightData lightingData)
{
    float3 lightPosition = lightingData.Position;
    float3 lightDir = lightingData.Direction;
    float cosInnerAngle = cos(lightingData.InnerAngle);
    float cosOuterAngle = cos(lightingData.OuterAngle);
    const float3 att = float3(0.01f, 0.01f, 0.01f);
    
    float3 toLight = lightPosition - surfPos;
    float distToLight = length(toLight);
    toLight /= distToLight;
    
    float3 mult = float3(1.0f, distToLight, distToLight * distToLight);

    float spotAttn = smoothstep(cosOuterAngle, cosInnerAngle, dot(-toLight, lightDir));
    float attn = (spotAttn * lightingData.Power / dot(att, mult));
    
    return CalculateBlinnPhong(toLight, surfPos, surfNormal) * attn;
}

float CalcShadowMultiplierForSpotLight(float3 surfPos, SpotLightData lightData, Texture2D shadowMap)
{
    float4 lightPerspectivePos = mul(lightData.VP, float4(surfPos, 1));
    lightPerspectivePos.xyz /= lightPerspectivePos.w;
    lightPerspectivePos.xy = float2((lightPerspectivePos.x + 1.0f) * 0.5f, 1.0f - (lightPerspectivePos.y + 1.0f) * 0.5f);
    return shadowMap.SampleCmpLevelZero(ComparisonSamplerState, lightPerspectivePos.xy, lightPerspectivePos.z - 0.0001f);
}

float CalcShadowMultiplierForPointLight(float3 surfPos, PointLightData lightData, TextureCube shadowMap)
{    
    float3 toPixel = surfPos - lightData.Position;
    float3 toPixelAbs = abs(toPixel);
    float z = max(toPixelAbs.x, max(toPixelAbs.y, toPixelAbs.z));
    
    float depth = (lightData.ProjectionA * z + lightData.ProjectionB) / z;
    
    return shadowMap.SampleCmpLevelZero(ComparisonSamplerState, toPixel.xzy, depth - 0.00001f);
}

float CalcShadowMultiplierForDirectionalLight(float3 surfPos, DirectionalLightData lightData, Texture2D shadowMap)
{
    float4 lightPerspectivePos = mul(lightData.VP, float4(surfPos, 1));
    lightPerspectivePos.xy = float2((lightPerspectivePos.x + 1.0f) * 0.5f, 1.0f - (lightPerspectivePos.y + 1.0f) * 0.5f);
    return shadowMap.SampleCmpLevelZero(ComparisonSamplerState, lightPerspectivePos.xy, lightPerspectivePos.z - 0.001f);
}

#endif // __LIGHTING_HEADER__