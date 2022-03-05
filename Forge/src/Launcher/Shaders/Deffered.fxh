#ifndef __DEFFERED_HEADER__
#define __DEFFERED_HEADER__

#include "Lighting.fxh"

cbuffer cbDefferedRendering : register(b4)
{
    float4x4 InvVP;
    float3 AmbientLighting;
};

struct PS_Output
{
    float4 Target : SV_Target0;
    float4 Diffuse : SV_TARGET1;
    float4 Normal : SV_TARGET2;
};

Custom_VS_Output Vert(Custom_VS_Input input);

Custom_VS_Output VS(Custom_VS_Input input)
{
    Custom_VS_Output output = Vert(input);
    __CALC_WORLD_POS__

    return output;
}

float4 CalculateColor(Custom_VS_Output input);

PS_Output PS(Custom_VS_Output input)
{
    PS_Output output;
    output.Diffuse = CalculateColor(input);
    output.Target = output.Diffuse * float4(CalcAmbient(input.Normal, AmbientLighting, AmbientLighting * 0.5f), 1.0f);
    output.Normal = float4((input.Normal + 1.0f) * 0.5f, 1.0f);
    
    return output;
}
#endif // __DEFFERED_HEADER__