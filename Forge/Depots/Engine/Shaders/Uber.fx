#include "Common.fxh"

#define __DEFINE_WORLD_POS__

#include "GeometryCommon.fxh"

struct Custom_VS_Input : VS_INPUT
{
    float2 TexCoord : TEXCOORD;
};

struct Custom_VS_Output : VS_OUTPUT
{
    float2 TexCoord : TEXCOORD1;
};

#ifdef __DEFERRED__
#include "Deferred.fxh"
#else
#include "Forward.fxh"
#endif

cbuffer cbMaterial : register(b1)
{
    float4 diffuseColor;
};

Custom_VS_Output Vert(Custom_VS_Input input)
{
    Custom_VS_Output output;

    float4x4 WVP = mul(VP, W);
    output.Pos = mul(WVP, float4(input.Pos, 1.0f));
    output.TexCoord = input.TexCoord;
    
    return output;
}

float4 CalculateColor(Custom_VS_Output input)
{    
    float4 clr = diffuseColor;
    
#ifdef __DIFFUSE_TEXTURE__
    clr = DiffuseTexture.Sample(LinearSamplerState, input.TexCoord);
#endif
    
 #ifdef __ALPHA_TEXTURE__
    clip(AlphaTexture.Sample(LinearSamplerState, input.TexCoord).r - 0.001f);
#endif
    
    return clr;
}