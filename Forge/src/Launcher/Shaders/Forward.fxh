#ifndef __FORWARD_HEADER__
#define __FORWARD_HEADER__

#include "Lighting.fxh"

cbuffer cbForwardRendering : register(b4)
{
    float3 AmbientLighting;
};

cbuffer cbForwardLighting : register(b5)
{
    LightData LightingData;
};

Custom_VS_Output Vert(Custom_VS_Input input);

Custom_VS_Output VS(Custom_VS_Input input)
{
    Custom_VS_Output output = Vert(input);
    __CALC_WORLD_POS__
    return output;
}

float4 CalculateColor(Custom_VS_Output input);

float4 PS(Custom_VS_Output input) : SV_Target
{
    float4 color = CalculateColor(input);
    
#ifdef __AMBIENT_LIGHT__
    color *= float4(CalcAmbient(input.Normal, AmbientLighting, AmbientLighting * 0.5f), 1.0f);
#elif defined __POINT_LIGHT__
    color *= float4( LightingData.Color * CalcPointLight(input.WorldPos, input.Normal, LightingData ), 1.0f );
#endif
    
    return color;
}
#endif // __FORWARD_HEADER__