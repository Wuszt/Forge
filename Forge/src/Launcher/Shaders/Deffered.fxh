#ifndef __DEFFERED_HEADER__
#define __DEFFERED_HEADER__

struct PS_Output
{
    float4 Color : SV_Target0;
    float4 Normal : SV_Target1;
};

OUTPUT Vert(INPUT input);

OUTPUT VS(INPUT input)
{
    return Vert(input);
}

float4 CalculateColor(OUTPUT input);

PS_Output PS(OUTPUT input) : SV_TARGET
{
    PS_Output output;
    output.Color = CalculateColor(input);
    output.Normal = float4(input.Normal, 1.0f);
    
    return output;
}
#endif // __DEFFERED_HEADER__