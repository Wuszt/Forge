#ifndef __COMMON_HEADER__
#define __COMMON_HEADER__

#define FORGE_PI 3.141592653589793238462643383279502884197169f
#define FORGE_PI_HALF 1.57079632679f
#define FORGE_PI_TWO 6.28318530718f
#define DEG2RAD 0.01745329251f

SamplerState LinearSamplerState : register(s0);
SamplerComparisonState ComparisonSamplerState : register(s1);

cbuffer cbFrame : register(b0)
{
    float Time;
};

cbuffer cbMesh : register(b2)
{
    float4x4 W;
};

cbuffer cbCamera : register(b3)
{
    float4x4 VP;
    float3 CameraPosition;
    float ProjectionA;
    float3 CameraDir;
    float ProjectionB;
    float NearPlane;
    float FarPlane;
};

float CalculateLinearDepth(float ProjectionA, float ProjectionB, float nonLinearDepth)
{
#if defined __NON_LINEAR_DEPTH__ || defined __PERSPECTIVE_CAMERA__
    return ProjectionB / (nonLinearDepth - ProjectionA);
#else
    return nonLinearDepth;
#endif
}

#endif // __COMMON_HEADER__