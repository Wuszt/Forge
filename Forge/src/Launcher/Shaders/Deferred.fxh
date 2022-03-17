#ifndef __DEFERRED_HEADER__
#define __DEFERRED_HEADER__

#include "Lighting.fxh"

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

PS_Output PS(Custom_VS_Output input)
{
    PS_Output output;
    output.Diffuse = CalculateColor(input);
    
#ifdef __NORMAL_TEXTURE__ 
    output.Normal = mul(W,NormalTexture.Sample(LinearSamplerState, input.TexCoord));
#else
    output.Normal = float4((input.Normal + 1.0f) * 0.5f, 1.0f);
#endif
       
    output.Target = output.Diffuse * float4(CalcAmbient(output.Normal.xyz * 2.0f - 1.0f, AmbientLighting, AmbientLighting * 0.5f), 1.0f);
    
    return output;
}
#endif // __DEFERRED_HEADER__