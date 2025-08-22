#include "Common.fxh"

#define __DEFINE_WORLD_POS__

#include "GeometryCommon.fxh"

struct Custom_VS_Input : VS_INPUT
{
#ifdef __VERTEX_INPUT_TEXCOORD_AMOUNT__
    float2 TexCoord : TEXCOORD;
#endif
};

struct Custom_VS_Output : VS_OUTPUT
{
#ifdef __VERTEX_INPUT_TEXCOORD_AMOUNT__
    float2 TexCoord : TEXCOORD1;
#endif
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

    float4 pos = float4(input.Pos, 1.0f);

    output.Pos = CalculateOutputPos(pos);

#ifdef __VERTEX_INPUT_TEXCOORD_AMOUNT__
    output.TexCoord = input.TexCoord;
#endif

#if !defined __NORMAL_TEXTURE__ && defined __VERTEX_INPUT_NORMAL_AMOUNT__ 
    output.Normal = input.Normal;
#endif

#ifdef __VERTEX_INPUT_COLOR_AMOUNT__
    output.Color = input.Color;
#endif
    
    return output;
}

float4 CalculateColor(Custom_VS_Output input)
{    
    float4 clr = diffuseColor;

#ifdef __VERTEX_INPUT_COLOR_AMOUNT__
    clr *= input.Color;
#endif
    
#ifdef __VERTEX_INPUT_TEXCOORD_AMOUNT__

#ifdef __DIFFUSE_TEXTURE__
    clr.rgb = DiffuseTexture.Sample(LinearSamplerState, input.TexCoord).rgb;
#endif
    
 #ifdef __ALPHA_TEXTURE__
    clip(AlphaTexture.Sample(LinearSamplerState, input.TexCoord).r - 0.001f);
#endif

#endif
    
    return clr;
}