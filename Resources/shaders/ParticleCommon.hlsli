
//ÉâÉìÉ_ÉÄ
float random(float2 p, float extend = 1.0f);
float random(float2 p, float extend)
{
    return frac(sin(dot(p, float2(12.9898f, 78.233f))) * (43758.5453f + extend));
}

// âÒì]çsóÒÇÇ©ÇØÇÈ
float4 rotate3(float4 pos, float4 axis, float angle)
{
    float4 nAxis = normalize(axis);
    return mul(float4x4(cos(angle) + pow(nAxis.x, 2) * (1 - cos(angle)), nAxis.x * nAxis.y * (1 - cos(angle)) - nAxis.z * sin(angle), nAxis.x * nAxis.z * (1 - cos(angle)) + nAxis.y * sin(angle), 0,
                                nAxis.y * nAxis.x * (1 - cos(angle)) + nAxis.z * sin(angle), cos(angle) + pow(nAxis.y, 2) * (1 - cos(angle)), nAxis.y * nAxis.z * (1 - cos(angle)) - nAxis.x * sin(angle), 0,
                                nAxis.z * nAxis.x * (1 - cos(angle)) - nAxis.y * sin(angle), nAxis.z * nAxis.y * (1 - cos(angle)) + nAxis.x * sin(angle), cos(angle) + pow(nAxis.z, 2) * (1 - cos(angle)), 0,
                                0, 0, 0, 1
                    ), pos);
};