#ifndef __FORWARD_HEADER__
#define __FORWARD_HEADER__

#include "Lighting.fxh"
#include "RenderingPassCommon.fxh"

cbuffer cbForwardRendering : register(b4)
{
    float3 AmbientLighting;
};

#if defined __POINT_LIGHT__ || defined __SPOT_LIGHT__ || defined __DIRECTIONAL_LIGHT__
cbuffer cbForwardLighting : register(b5)
{
    LightData LightingData;
};
#endif

#if defined __SPOT_LIGHT__ || defined __DIRECTIONAL_LIGHT__
Texture2D ShadowMap : register(t31);
#elif defined __POINT_LIGHT__
TextureCube ShadowMap : register(t31);
#endif

float4 CalculateColor(Custom_VS_Output input);

float4 PS(Custom_VS_Output input) : SV_Target
{    
    float4 color = CalculateColor(input);
    float3 normal = CalculateNormal(input).xyz;

#if defined __SPOT_LIGHT__
    color *= CalcShadowMultiplierForSpotLight( input.WorldPos, LightingData, ShadowMap );
#elif defined __DIRECTIONAL_LIGHT__
    color *= CalcShadowMultiplierForDirectionalLight( input.WorldPos, LightingData, ShadowMap );
#elif defined __POINT_LIGHT__
    color *= CalcShadowMultiplierForPointLight( input.WorldPos, LightingData, ShadowMap );
#endif
    
#ifdef __AMBIENT_LIGHT__
    color *= float4(CalcAmbient(normal, AmbientLighting, AmbientLighting * 0.5f), 1.0f);
#elif defined __POINT_LIGHT__
    color *= float4( LightingData.Color * CalcPointLight(input.WorldPos, normal, LightingData ), 1.0f );
#elif defined __SPOT_LIGHT__
    color *= float4( LightingData.Color * CalcSpotLight(input.WorldPos, normal, LightingData ), 1.0f );
    #elif defined __DIRECTIONAL_LIGHT__
    color *= float4( LightingData.Color * CalcDirectionalLight(input.WorldPos, normal, LightingData ), 1.0f );
#endif
    
    return color;
}
#endif // __FORWARD_HEADER__