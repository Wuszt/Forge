#ifndef __LIGHTING_HEADER__
#define __LIGHTING_HEADER__

#include "Common.fxh"

float3 CalcAmbient(float3 normal)
{
    float3 ambientUp = float3(0.2f, 0.2f, 0.2f);
    float3 ambientDown = float3(0.15f, 0.15f, 0.15f);
    float a = (1.0f + normal.z) * 0.5f;
    return ambientUp * a + (1.0f - a) * ambientDown;
}

float3 CalculateBlinnPhong(float3 toLight, float3 position, float3 normal)
{
    float3 diffuseColor = float3(1.0f, 1.0f, 1.0f);
    float3 specularColor = float3(1.0f, 1.0f, 1.0f);
    
    
    float3 toCamera = normalize(CameraPosition - position);
    
    float3 H = normalize(toLight + toCamera);
    
    float diffuse = saturate(dot(toLight, normal));
    float specular = saturate(dot(H, normal));
    specular = pow(specular, 16.0f);
    return saturate(diffuseColor * diffuse + specularColor * specular);
}

float3 CalcDirectionalLight(float3 position, float3 normal)
{
    float3 toLight = normalize(float3(1.0f, 1.0f, 1.0f));
    return CalculateBlinnPhong(toLight, position, normal);
}

float3 CalcPointLight(float3 position, float3 normal)
{
    const float3 lightPosition = float3(30.0f, 1250.0f, 340.0f);
    const float3 att = float3(0.01f, 0.01f, 0.01f);
    const float lightPower = 400.0f;
    
    float3 toLight = lightPosition - position;
    float distToLight = length(toLight);
    toLight /= distToLight;
    
    float3 mult = float3(1.0f, distToLight, distToLight * distToLight);
    
    float attn = saturate(lightPower / dot(att, mult));
    return CalculateBlinnPhong(toLight, position, normal) * attn;
}

float3 CalcSpotLight(float3 position, float3 normal)
{
    float3 lightPosition = CameraPosition;
    float3 lightDir = CameraDir;
    float cosInnerAngle = cos(FORGE_PI / 12.0f);
    float cosOuterAngle = cos(FORGE_PI / 6.0f);
    const float lightPower = 4000.0f;
    const float3 att = float3(0.01f, 0.01f, 0.01f);
    
    float3 toLight = lightPosition - position;
    float distToLight = length(toLight);
    toLight /= distToLight;
    
    float3 mult = float3(1.0f, distToLight, distToLight * distToLight);

    float spotAttn = smoothstep(cosOuterAngle, cosInnerAngle, dot(-toLight, lightDir));
    float attn = (spotAttn * lightPower / dot(att, mult));
    
    return CalculateBlinnPhong(toLight, position, normal) * attn;
}

float3 CalculateLighting(float3 position, float3 normal)
{
    return CalcAmbient(normal) + CalcDirectionalLight(position, normal);
}
#endif // __LIGHTING_HEADER__