/*
 * "Seascape" by Alexander Alekseev aka TDM - 2014
 * License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
 */

#include"PeraShaderHeader.hlsli"

// math
float3x3 fromEuler(float3 ang)
{
    float2 a1 = float2(sin(ang.z), -cos(ang.z));
    float2 a2 = float2(sin(ang.x), cos(ang.x));
    float2 a3 = float2(sin(ang.y), cos(ang.y));
    float3x3 m;
    m[0] = float3(a1.y * a3.y + a1.x * a2.x * a3.x, a1.y * a2.x * a3.x + a3.y * a1.x, -a2.y * a3.x);
    m[1] = float3(-a2.y * a1.x, a1.y * a2.y, a2.x);
    m[2] = float3(a3.y * a1.x * a2.x + a1.y * a3.x, a1.x * a3.x - a1.y * a3.y * a2.x, a2.y * a3.y);
    return m;
}

//-------------------------------------

static const int NUM_STEPS = 8;
static const float PI = 3.141592;
static const float EPSILON = 1e-3;
static const float TIME_EXTEND = 0.04f;
static const float2 IRESO = { 1280.0f, 720.0f };

#define EPSILON_NRM (0.1 / IRESO.x)
#define SEA_TIME (1.0 + time*TIME_EXTEND * SEA_SPEED*seaTimeExtend)

// sea
static const int ITER_GEOMETRY = 3;
static const int ITER_FRAGMENT = 5;
static const float SEA_HEIGHT = 0.6;
static const float SEA_CHOPPY = 4.0;
static const float SEA_SPEED = 0.8;
static const float SEA_FREQ = 0.16;
static const float3 SEA_BASE = float3(0.1, 0.19, 0.22);
static const float3 SEA_WATER_COLOR = float3(0.8, 0.9, 0.6);
static const float2x2 octave_m = float2x2(1.6, 1.2, -1.2, 1.6);

// Get random value.
float hash(float2 p)
{
    float h = dot(p, float2(127.1, 311.7));
    return frac(sin(h) * 43758.5453123);
}

// Get Noise.
float noiseS(float2 p)
{
    float2 i = floor(p);
    float2 f = frac(p);

    // u = -2.0f^3 + 3.0f^2
    float2 u = f * f * (3.0 - 2.0 * f);

    // Get each grid vertices.
    // +---+---+
    // | a | b |
    // +---+---+
    // | c | d |
    // +---+---+
    float a = hash(i + float2(0.0, 0.0));
    float b = hash(i + float2(1.0, 0.0));
    float c = hash(i + float2(0.0, 1.0));
    float d = hash(i + float2(1.0, 1.0));

    // Interpolate grid parameters with x and y.
    float result = lerp(lerp(a, b, u.x),
                        lerp(c, d, u.x), u.y);

    // Normalized to '-1 - 1'.
    return (2.0 * result) - 1.0;
}

// lighting
float diffuse(float3 n, float3 l, float p)
{
    return pow(dot(n, l) * 0.4 + 0.6, p);
}

float specular(float3 n, float3 l, float3 e, float s)
{
    float nrm = (s + 8.0) / (PI * 8.0);
    return pow(max(dot(reflect(e, n), l), 0.0), s) * nrm;
}

// Get sky color by 'eye' position.
// So, The color changed smoothly by eye level.
float3 getSkyColor(float3 e)
{
    e.y = max(e.y, 0.0);
    float r = pow(1.0 - e.y, 2.0);
    float g = 1.0 - e.y;
    float b = 0.6 + (1.0 - e.y) * 0.4;
    return float3(r, g, b) * seaSkyCol;
}

// Get sea wave octave.
float sea_octave(float2 uv, float choppy)
{
    uv += noiseS(uv);
    float2 wv = 1.0 - abs(sin(uv));
    float2 swv = abs(cos(uv));
    wv = lerp(wv, swv, wv);
    return pow(1.0 - pow(wv.x * wv.y, 0.65), choppy);
}

// p is ray position.
float map(float3 p)
{
    float freq = SEA_FREQ; // => 0.16
    float amp = SEA_HEIGHT; // => 0.6
    float choppy = SEA_CHOPPY; // => 4.0

    // XZ plane.
    float2 uv = p.xz;

    float d, h = 0.0;

    // ITER_GEOMETRY => 3
    for (int i = 0; i < ITER_GEOMETRY; i++)
    {
        d = sea_octave((uv + SEA_TIME) * freq, choppy);
        d += sea_octave((uv - SEA_TIME) * freq, choppy);
        h += d * amp;
        uv = mul(octave_m, uv);
        freq *= 2.0;
        amp *= 0.2;
        choppy = lerp(choppy, 1.0, 0.2 * seaTimeExtend);
    }

    return p.y - h;
}

// p is ray position.
// This function calculate detail map with more iteration count.
float map_detailed(float3 p)
{
    float freq = SEA_FREQ;
    float amp = SEA_HEIGHT;
    float choppy = SEA_CHOPPY;

    float2 uv = p.xz;

    float d, h = 0.0;

    // ITER_FRAGMENT = 5
    for (int i = 0; i < ITER_FRAGMENT; i++)
    {
        d = sea_octave((uv + SEA_TIME) * freq, choppy);
        d += sea_octave((uv - SEA_TIME) * freq, choppy);
        h += d * amp;
        uv = mul(octave_m, uv);
        freq *= 2.0;
        amp *= 0.2;
        choppy = lerp(choppy, 1.0, 0.2);
    }

    return p.y - h;
}

// p = ray position
// n = surface normal
// l = light
// eye = eye
// dist = ray marching distance
float3 getSeaColor(float3 p, float3 n, float3 l, float3 eye, float3 dist)
{
    float fresnel = clamp(1.0 - dot(n, -eye), 0.0, 1.0);
    fresnel = pow(fresnel, 3.0) * 0.65;

    float3 reflected = getSkyColor(reflect(eye, n));
    float3 refracted = SEA_BASE + diffuse(n, l, 80.0) * SEA_WATER_COLOR * 0.12;

    float3 colorL = lerp(refracted, reflected, fresnel);

    float atten = max(1.0 - dot(dist, dist) * 0.001, 0.0);
    colorL += SEA_WATER_COLOR * (p.y - SEA_HEIGHT) * 0.18 * atten;

    float s = specular(n, l, eye, 60.0);
    colorL += float3(s, s, s);

    return colorL * seaCol;
}

// tracing
float3 getNormal(float3 p, float eps)
{
    float3 n;
    n.y = map_detailed(p);
    n.x = map_detailed(float3(p.x + eps, p.y, p.z)) - n.y;
    n.z = map_detailed(float3(p.x, p.y, p.z + eps)) - n.y;
    n.y = eps;
    return normalize(n);
}

// Get Height Map
float3 heightMapTracing(float3 ori, float3 dir)
{
    float tm = 0.0;

    float tx = 1000.0;

    float3 p = 0;
    
    // Calculate 1000m far distance map.
    float hx = map(ori + dir * tx);

    // if hx over 0.0 is that ray on the sky. right?
    if (hx > 0.0)
    {
        p = float3(0, 0, 0);
        return tx;
    }

    float hm = map(ori + dir * tm);
    float tmid = 0.0;

    // NUM_STEPS = 8
    for (int i = 0; i < NUM_STEPS; i++)
    {
        // Normalized by max distance (hx is max far position), is it correct?
        float f = hm / (hm - hx);

        tmid = lerp(tm, tx, f);
        p = ori + dir * tmid;

        float hmid = map(p);

        if (hmid < 0.0)
        {
            tx = tmid;
            hx = hmid;
        }
        else
        {
            tm = tmid;
            hm = hmid;
        }
    }

    return p;
}

// main
float4 mainImage(float2 fragCoord)
{
    float2 uv = fragCoord.xy / (IRESO.xy * resoulution);
    uv = uv * 2.0 - 1.0;

    const float3 light = normalize(float3(0.0, 1.0, 0.8));

    // ray 
    float3x3 rotM = fromEuler(seaDirRot * seaDirRotExtend);
    
    float3 ori = float3(seaCameraPos.z, seaCameraPos.y, seaCameraPos.x);
    float3 dir = normalize(float3(uv.x, uv.y, 4.0f));
    //dir.z += length(uv);
    dir = mul(rotM, dir);

    // tracing
    float3 p;
    p = heightMapTracing(ori, dir);

    float3 dist = p - ori;
    float3 n = getNormal(p, dot(dist, dist) * EPSILON_NRM);

    // color
    float3 sky = getSkyColor(dir);
    float3 sea = getSeaColor(p, n, light, dir, dist);

    // This is coefficient for smooth blending sky and sea with 
    float t = pow(smoothstep(0.0, -0.05, dir.y), 0.3);
    float3 colorL = lerp(sky, sea, t);

    // post
    return float4(colorL, 1.0);
}