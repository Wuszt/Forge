#ifndef __GEOMETRY_COMMON_HEADER__
#define __GEOMETRY_COMMON_HEADER__

#include "Common.fxh"

struct VS_INPUT
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
};

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float3 Normal : NORMAL;
};
#endif