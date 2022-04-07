#ifndef __FORWARD_HEADER__
#define __FORWARD_HEADER__

#include "Lighting.fxh"

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

#if defined __SPOT_LIGHT__
Texture2D ShadowMap : register(t31);
#elif defined __POINT_LIGHT__
TextureCube ShadowMap : register(t31);
#endif

Custom_VS_Output Vert(Custom_VS_Input input);

Custom_VS_Output VS(Custom_VS_Input input)
{
    Custom_VS_Output output = Vert(input);
    __CALC_WORLD_POS__
    
#ifndef __NORMAL_TEXTURE__
    output.Normal = normalize(mul(W, float4(input.Normal, 0.0f)).xyz);
#endif
    
    return output;
}

float4 CalculateColor(Custom_VS_Output input);

float4 PS(Custom_VS_Output input) : SV_Target
{    
    float4 color = CalculateColor(input);

    float3 normal;
    
#ifdef __NORMAL_TEXTURE__
    normal = mul(W,(NormalTexture.Sample(LinearSamplerState, input.TexCoord) + 1.0f) * 0.5f).rgb;
#else
    normal = input.Normal;
#endif
    
#ifdef __SPOT_LIGHT__
    color *= CalcShadowMultiplierFromTexture( input.WorldPos, LightingData, ShadowMap );
#elif defined __POINT_LIGHT__
    color *= CalcShadowMultiplierFromCube( input.WorldPos, LightingData, ShadowMap );
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