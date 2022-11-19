#ifndef __DEFERRED_HEADER__
#define __DEFERRED_HEADER__

#include "Lighting.fxh"
#include "RenderingPassCommon.fxh"

cbuffer cbDeferredRendering : register(b4)
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

float4 CalculateColor(Custom_VS_Output input);

PS_Output PS(Custom_VS_Output input)
{
    PS_Output output;
    output.Diffuse = CalculateColor(input);

    output.Normal = (CalculateNormal(input) + 1.0f) * 0.5f;
       
    output.Target = output.Diffuse * float4(CalcAmbient(output.Normal.xyz * 2.0f - 1.0f, AmbientLighting, AmbientLighting * 0.5f), 1.0f);
    
    return output;
}
#endif // __DEFERRED_HEADER__