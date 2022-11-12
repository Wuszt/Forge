#include "Common.fxh"

#define __DEFINE_WORLD_POS__

#include "GeometryCommon.fxh"

#define ON 1

struct Custom_VS_Input : VS_INPUT
{
    float2 TexCoord : TEXCOORD;
#if ON
    float4 BlendWeights : BLENDWEIGHTS;
    uint4 BlendIndices : BLENDINDICES;
#endif
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

cbuffer cbBones : register(b6)
{
    float4x4 Bones[100];
}

Custom_VS_Output Vert(Custom_VS_Input input)
{
    Custom_VS_Output output;

    float4x4 WVP = mul(VP, W);

    float4 pos = float4(input.Pos, 1.0f);
#if ON
    float4x4 boneTransform = Bones[input.BlendIndices[0]] * input.BlendWeights[0];
    boneTransform += Bones[input.BlendIndices[1]] * input.BlendWeights[1];
    boneTransform += Bones[input.BlendIndices[2]] * input.BlendWeights[2];
    boneTransform += Bones[input.BlendIndices[3]] * input.BlendWeights[3];

    output.TexCoord = 0.0f;

    float sum = input.BlendWeights.x + input.BlendWeights.y + input.BlendWeights.z + input.BlendWeights.w;
    if(sum == 0.0f)
    {
        boneTransform = Bones[input.BlendIndices[0]] * 0.25f;
        boneTransform += Bones[input.BlendIndices[1]] * 0.25f;
        boneTransform += Bones[input.BlendIndices[2]] * 0.25f;
        boneTransform += Bones[input.BlendIndices[3]] * 0.25f;
    }

    pos = mul(boneTransform, pos);
#endif

    //output.Pos = pos;
    output.Pos = mul(WVP, pos);
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