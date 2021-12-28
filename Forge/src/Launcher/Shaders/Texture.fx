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
};

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
    float2 TexCoord : TEXCOORD;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;

    float4x4 WVP = mul(VP, W);
    output.Pos = mul(WVP, float4(input.Pos, 1.0f));
    output.Color = diffuseColor;
    output.TexCoord = input.TexCoord;

    return output;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    return shaderTexture.Sample(LinearSamplerState, input.TexCoord);

}