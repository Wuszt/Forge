#include "FullScreenCommon.fxh"

Texture2D Source : register(t0);

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    float2 texCoord = float2((input.UV.x + 1.0f) * 0.5f, 1.0f - (input.UV.y + 1.0f) * 0.5f);
    return Source.Sample(LinearSamplerState, texCoord);
}