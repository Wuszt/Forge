#ifndef __GEOMETRY_COMMON_HEADER__
#define __GEOMETRY_COMMON_HEADER__

#include "Common.fxh"

#ifndef __DEFERRED__
#define __FORWARD__
#endif

#ifdef __FORWARD__
#define __DEFINE_WORLD_POS__ 
#endif

struct VS_INPUT
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
};

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float3 Normal : NORMAL;
#ifdef __DEFINE_WORLD_POS__
    float3 WorldPos : TEXCOORD0;
#endif
};

#ifdef __DEFINE_WORLD_POS__
#define __CALC_WORLD_POS__ output.WorldPos = mul(W, float4(input.Pos, 1.0f)).xyz;
#else
#define __CALC_WORLD_POS__
#endif

#endif