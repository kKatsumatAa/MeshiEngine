

//‰ñ“]‚³‚¹‚é
//p‚Í‰ñ“]‚³‚¹‚½‚¢À•W@angle‚Í‰ñ“]‚³‚¹‚éŠp“x@axis‚Í‚Ç‚Ì²‚ğŒ³‚É‰ñ“]‚³‚¹‚é‚©@
float3 rotate(float3 p, float angle, float3 axis)
{
    float3 a = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float r = 1.0 - c;
    float3x3 m = float3x3(
                    a.x * a.x * r + c, a.y * a.x * r + a.z * s, a.z * a.x * r - a.y * s,
                    a.x * a.y * r - a.z * s, a.y * a.y * r + c, a.z * a.y * r + a.x * s,
                    a.x * a.z * r + a.y * s, a.y * a.z * r - a.x * s, a.z * a.z * r + c
                );

    return mul(m, p);
}

//ƒ‰ƒ“ƒ_ƒ€
float random(float2 p, float extend = 1.0f);
float random(float2 p, float extend)
{
    return frac(sin(dot(p, float2(12.9898f, 78.233f))) * (43758.5453f + extend));
}