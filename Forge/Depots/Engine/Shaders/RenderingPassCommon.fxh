#ifndef __RENDERINGPASS_COMMON_HEADER__
#define __RENDERINGPASS_COMMON_HEADER__

#include "GeometryCommon.fxh"

cbuffer cbBones : register( b6 )
{
    float4x4 Bones[70];
}

Custom_VS_Output Vert(Custom_VS_Input input);

float4 CalculateNormal(Custom_VS_Output vsOutput)
{
#ifdef __NORMAL_TEXTURE__
    float4 normal = NormalTexture.Sample(LinearSamplerState, vsOutput.TexCoord) + 1.0f) * 0.5f;

#if defined __VERTEX_INPUT_BLENDWEIGHTS_AMOUNT__ && defined __VERTEX_INPUT_BLENDINDICES_AMOUNT__
    normal = mul( vsOutput.boneTransform, normal );
#endif
#else
    float4 normal = float4(vsOutput.Normal, 0.0f);
#endif

    normal = mul(W, normal);

    normal = normalize(normal);
    return normal;
}

Custom_VS_Output VS( Custom_VS_Input input )
{
#if defined __VERTEX_INPUT_BLENDWEIGHTS_AMOUNT__ && defined __VERTEX_INPUT_BLENDINDICES_AMOUNT__

#ifdef __NORMAL_TEXTURE__
    #define BONETRANSFORM output.boneTransform
#else
    float4x4 boneTransform;
    #define BONETRANSFORM boneTransform
#endif

    BONETRANSFORM = Bones[ input.BlendIndices[ 0 ] ] * input.BlendWeights[ 0 ];
    BONETRANSFORM += Bones[ input.BlendIndices[ 1 ] ] * input.BlendWeights[ 1 ];
    BONETRANSFORM += Bones[ input.BlendIndices[ 2 ] ] * input.BlendWeights[ 2 ];
    BONETRANSFORM += Bones[ input.BlendIndices[ 3 ] ] * input.BlendWeights[ 3 ];
    input.Pos = mul(BONETRANSFORM, float4(input.Pos, 1.0f)).xyz;

#ifndef __NORMAL_TEXTURE__
    input.Normal = mul(BONETRANSFORM, float4(input.Normal, 0.0f)).xyz;
#endif

#endif

    Custom_VS_Output output = Vert(input);
    __CALC_OUTPUT_WORLD_POS__(float4( input.Pos, 1.0f ));

    return output;
}

#endif