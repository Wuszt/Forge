#include "Common.fxh"

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
    float4 Color : COLOR;
    float3 Normal : NORMAL;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;

    float4x4 WVP = mul(VP, W);
    output.Pos = mul(WVP, float4(input.Pos, 1.0f));
    output.Color = diffuseColor;
    output.Normal = normalize(mul(W, float4(input.Normal, 1.0f)).xyz);

    return output;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    return float4(CalcAmbient(input.Normal, input.Color.rgb), 1.0f);
}