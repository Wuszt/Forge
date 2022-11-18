#ifndef __RENDERINGPASS_COMMON_HEADER__
#define __RENDERINGPASS_COMMON_HEADER__

#include "GeometryCommon.fxh"

cbuffer cbBones : register( b6 )
{
    float4x4 Bones[70];
}

Custom_VS_Output Vert( Custom_VS_Input input );

Custom_VS_Output VS( Custom_VS_Input input )
{
#if defined __VERTEX_INPUT_BLENDWEIGHTS_AMOUNT__ && defined __VERTEX_INPUT_BLENDINDICES_AMOUNT__
    float4x4 boneTransform = Bones[ input.BlendIndices[ 0 ] ] * input.BlendWeights[ 0 ];
    boneTransform += Bones[ input.BlendIndices[ 1 ] ] * input.BlendWeights[ 1 ];
    boneTransform += Bones[ input.BlendIndices[ 2 ] ] * input.BlendWeights[ 2 ];
    boneTransform += Bones[ input.BlendIndices[ 3 ] ] * input.BlendWeights[ 3 ];
    input.Pos = mul(boneTransform, float4(input.Pos, 1.0f)).xyz;
#endif

    Custom_VS_Output output = Vert( input );
    __CALC_OUTPUT_WORLD_POS__( float4( input.Pos, 1.0f ) );

#ifndef __NORMAL_TEXTURE__
    output.Normal = normalize( mul( W, float4( input.Normal, 0.0f ) ).xyz );
#endif

    return output;
}

#endif