#ifndef __FULLSCREEN_COMMON_HEADER__
#define __FULLSCREEN_COMMON_HEADER__

#include "Common.fxh"

struct VS_OUTPUT
{
    float4 Position : SV_POSITION; 
    float2 UV : TEXCOORD0;
};

#endif // __FULLSCREEN_COMMON_HEADER__