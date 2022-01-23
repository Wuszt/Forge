#include "GeometryCommon.fxh"
#include "Lighting.fxh"

struct INPUT : VS_INPUT
{
    float2 TexCoord : TEXCOORD;
};

struct OUTPUT : VS_OUTPUT
{
};

#define __DEFFERED__
#ifdef __DEFFERED__
#include "Deffered.fxh"
#endif

cbuffer cbMaterial : register(b1)
{
    float4 diffuseColor;
};

OUTPUT Vert(INPUT input)
{
    OUTPUT output;

    float4x4 WVP = mul(VP, W);
    output.Pos = mul(WVP, float4(input.Pos, 1.0f));
    output.Normal = normalize(mul(W, float4(input.Normal, 1.0f)).xyz);

    return output;
}

float4 CalculateColor(OUTPUT input)
{
    float4 clr = diffuseColor;
    clip(clr.a - 0.1f);
    
    return clr;
}