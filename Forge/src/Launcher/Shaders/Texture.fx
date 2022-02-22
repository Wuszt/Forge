#include "GeometryCommon.fxh"

struct Custom_VS_Input : VS_INPUT
{
    float2 TexCoord : TEXCOORD;
};

struct Custom_VS_Output : VS_OUTPUT
{
    float2 TexCoord : TEXCOORD0;
    float3 WorldPos : TEXCOORD1;
};

#define __DEFFERED__
#ifdef __DEFFERED__
#include "Deffered.fxh"
#endif

Texture2D ShaderTexture;
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
    output.Normal = normalize(mul(W, float4(input.Normal, 0.0f)).xyz);
    output.WorldPos = mul(W, float4(input.Pos, 1.0f)).xyz;

    return output;
}

float4 CalculateColor(Custom_VS_Output input)
{
    float4 clr = ShaderTexture.Sample(LinearSamplerState, input.TexCoord);
    clip(clr.a - 0.1f);
    
    return clr;
}