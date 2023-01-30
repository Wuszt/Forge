#include "GeometryCommon.fxh"

TextureCube Skybox : register( t0 );

struct SKYBOX_VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float3 UVs : TEXCOORD0;
};

SKYBOX_VS_OUTPUT VS( VS_INPUT input )
{
    SKYBOX_VS_OUTPUT output;
    output.Pos = mul( VP, float4( input.Pos, 0.0f ) );   
    output.UVs = input.Pos.xzy;
    output.Pos.z = output.Pos.w;
    return output;
}

float4 PS( SKYBOX_VS_OUTPUT input ) : SV_TARGET
{    
    return Skybox.Sample( LinearSamplerState, input.UVs );
}