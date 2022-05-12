#include "FullScreenCommon.fxh"

static const float2 arrBasePos[4] =
{
    float2(-1.0, 1.0),
    float2(-1.0, -1.0),
    float2(1.0, 1.0),
    float2(1.0, -1.0),
};

VS_OUTPUT VS(uint vI : SV_VERTEXID)
{
    VS_OUTPUT output;
    
    output.Position = float4(arrBasePos[vI].xy, 0.0, 1.0);
    output.UV = arrBasePos[vI].xy;
    
    return output;
}