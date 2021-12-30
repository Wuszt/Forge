#include "Common.fxh"

SamplerState LinearSamplerState : register(s0);
Texture2D shaderTexture;

cbuffer cbMaterial : register(b1)
{
    float4 diffuseColor;
};

struct VS_INPUT
{
    float3 Pos : POSITION;
    float2 TexCoord : TEXCOORD;
    float3 Normal : NORMAL;
};

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float3 WorldPos : TEXCOORD1;
    float3 Normal : NORMAL;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;

    float4x4 WVP = mul(VP, W);
    output.Pos = mul(WVP, float4(input.Pos, 1.0f));
    output.TexCoord = input.TexCoord;
    output.Pos = mul(WVP, float4(input.Pos, 1.0f));
    output.Normal = normalize(mul(W, float4(input.Normal, 1.0f)).xyz);
    output.WorldPos = mul(W, float4(input.Pos, 1.0f));

    return output;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    float4 clr = shaderTexture.Sample(LinearSamplerState, input.TexCoord);
    clip(clr.a - 0.1f);
    return clr * (float4(CalcAmbient(input.Normal), 1.0f) + float4(CalcPointLight(input.WorldPos, input.Normal), 1.0f));

}