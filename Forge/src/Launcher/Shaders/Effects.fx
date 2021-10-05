cbuffer cbMaterial : register(b1)
{
    float4 color;
};

cbuffer cbFrame : register(b0)
{
    float time;
};

cbuffer cbMesh : register(b2)
{
    float4x4 W;
    float4x4 VP;
};

struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float4 Color : COLOR;
};

VS_OUTPUT VS(float4 inPos : POSITION0, float4 inColor : COLOR )
{
    VS_OUTPUT output;

    float4x4 WVP = mul(VP, W);
    
    output.Pos = mul(WVP, inPos);
    output.Color = color;

    return output;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    return input.Color;
}