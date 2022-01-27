#ifndef __DEFFERED_HEADER__
#define __DEFFERED_HEADER__

struct PS_Output
{
    float4 Color : SV_TARGET0;
    float4 Normal : SV_TARGET1;
};

Custom_VS_Output Vert(Custom_VS_Input input);

Custom_VS_Output VS(Custom_VS_Input input)
{
    return Vert(input);
}

float4 CalculateColor(Custom_VS_Output input);

PS_Output PS(Custom_VS_Output input)
{
    PS_Output output;
    output.Color = CalculateColor(input);
    output.Normal = float4(input.Normal, 1.0f);
    
    return output;
}
#endif // __DEFFERED_HEADER__