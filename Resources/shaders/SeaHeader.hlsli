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

//�@���ɂ����鏬�����l
#define EPSILON_NRM (0.1 / IRESO.x)
#define SEA_TIME (1.0 + time*TIME_EXTEND * SEA_SPEED*seaTimeExtend)

// sea
static const int ITER_GEOMETRY = 3; //octave
static const int ITER_FRAGMENT = 5; //�ڍׂȒl�����߂邽�߂�octave
static const float SEA_HEIGHT = 0.6;
static const float SEA_CHOPPY = 4.0 * seaTimeExtend;
static const float SEA_SPEED = 0.8;
static const float SEA_FREQ = 0.16;
static const float3 SEA_BASE = float3(0.1, 0.19, 0.22);
static const float3 SEA_WATER_COLOR = float3(0.8, 0.9, 0.6);
static const float2x2 octave_m = float2x2(1.6, 1.2, -1.2, 1.6);

// Get random value.
//�^�������_���Ȃ̂œ����l����ꂽ�瓯���l���Ԃ��Ă���
float hash(float2 p)
{
    float h = dot(p, float2(127.1, 311.7));
    return frac(sin(h) * 43758.5453123);
}

// Get Noise.
//�o�����[�m�C�Y(�^��)
float noiseS(float2 p)
{
    //�����Ə����ɕ�����
    float2 i = floor(p);
    float2 f = frac(p);

    // Get each grid vertices.
    // +---+---+
    // | a | b |
    // +---+---+
    // | c | d |
    // +---+---+
    //�����������g���ăO���b�h������
    float a = hash(i + float2(0.0, 0.0));
    float b = hash(i + float2(1.0, 0.0));
    float c = hash(i + float2(0.0, 1.0));
    float d = hash(i + float2(1.0, 1.0));
    
    // u = -2.0f^3 + 3.0f^2
    //���������ŃO���b�h��⊮
    float2 u = f * f * (3.0 - 2.0 * f);

    // Interpolate grid parameters with x and y.
    //a,b c,d���������ɕ�ԁA���̌��ʂ��c�����ɕ��
    float result = lerp(lerp(a, b, u.x),
                        lerp(c, d, u.x), u.y);

    // Normalized to '-1 - 1'.
    //-1.0~1.0�ɕ␳���Ă���
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
    //�J�����̈ʒu��y���W�݂̂𗘗p
    e.y = max(e.y, 0.0);
    //�������߂��͔������āA��̕��ɍs���ɂ���������
    float r = pow(1.0 - e.y, 2.0);
    float g = 1.0 - e.y;
    float b = 0.6 + (1.0 - e.y) * 0.4;
    return float3(r, g, b) * seaSkyCol;
}

// Get sea wave octave.
float sea_octave(float2 uv, float choppy)
{
    //�m�C�Y�����Z
    uv += noiseS(uv);
    //�g��\�����邽�߂�sin,cos
    float2 wv = 1.0 - abs(sin(uv));
    float2 swv = abs(cos(uv));
    //���`��Ԃ��č���
    wv = lerp(wv, swv, wv);
    //���G�ɍ���(wv.x��Ԃ��Ă���������)
    return pow(1.0 - pow(wv.x * wv.y, 0.65), choppy);
}

// p is ray position.
float map(float3 p)
{
    float freq = SEA_FREQ; // ���g��
    float amp = SEA_HEIGHT; // �U��
    float choppy = SEA_CHOPPY; // �g��

    // XZ���ʂɂ��v�Z
    float2 uv = p.xz;

    float d, h = 0.0;

    // ITER_GEOMETRY = 3
    // �����Łu�t���N�^���u���E���^��(�����֐��ɔ����ɈႤ�������������ĉ����)�v�ɂ��m�C�Y�̌v�Z���s���Ă���
    //(�m�C�Y�𕡐��d�˂邱�ƂŎ���
    //�����g�������̊����ő���������ilacunarity�j�Ɠ����ɐU�������炵�Ȃ���igain�j�m�C�Y���ioctave�̐������j�J��Ԃ��d�˂�)
    for (int i = 0; i < ITER_GEOMETRY; i++)
    {
        // SEA_SPEED = 0.8
        // �P���ɁAiTime�i���Ԍo�߁j�ɂ����UV�l������ɂ��炵�ăA�j���[�V���������Ă���
        // iTime�݂̂��w�肵�Ă��قړ����A�j���[�V�����ɂȂ�
        //
        // SEA_TIME�̃v���X���ƃ}�C�i�X���𑫂��ĐU����傫�����Ă���E�E�E�H
        d = sea_octave((uv + SEA_TIME) * freq, choppy) * seaTimeExtend * 2.0f;
        d += sea_octave((uv - SEA_TIME) * freq, choppy) * seaTimeExtend * 2.0f;

        h += d * amp;

        // octave_m = mat2(1.6, 1.2, -1.2, 1.6);
        // uv�l����]�����Ă��镗�B������Ȃ����ƕ��R�ȊC�ɂȂ�
        uv = mul(octave_m, uv);

        // fbm�֐��Ƃ��āA�U����0.2�{�A���g����2.0�{���Ď��̌v�Z���s��
        freq *= 2.0;
        amp *= 0.2;

        // choppy��|�󂷂�Ɓu�g�p�v�Ƃ����Ӗ�
        // ���������������ƊC���u���ƂȂ����v�Ȃ�
        choppy = lerp(choppy, 1.0, 0.2);
    }

    // �Ō�ɁA���܂�������`h`���A���݂̃��C�̍���������������̂��u�g�̍����v�Ƃ��Ă���
    return p.y - h;
}

//map()�Ə����͈ꏏ�ŃC�e���[�V�����񐔂���������
//(�C�e���[�V�����񐔂��R���p�C�����Ɍ��܂��ĂȂ��Ⴂ���Ȃ����ߕ����Ă�)
float map_detailed(float3 p)
{
    float freq = SEA_FREQ; // ���g��
    float amp = SEA_HEIGHT; // �U��
    float choppy = SEA_CHOPPY; // �g��

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
    //�t���l���̎�(���������Ɩʂ̌����œ��ߗ��Ɣ��˗����ω�
    //���@���Ǝ��������ւ̃x�N�g���̓��ς���v����������ǂ������A���΂Ȃقǌi�F���f��)
    float fresnel = clamp(1.0 - dot(n, -eye), 0.0, 1.0);
    fresnel = pow(fresnel, 3.0) * 0.65;

    //���ܐ�̊C�̐F�A���ː�̋�̐F���v�Z
    float3 reflected = getSkyColor(reflect(eye, n));
    float3 refracted = SEA_BASE + diffuse(n, l, 80.0) * SEA_WATER_COLOR * 0.12;

    //���`���
    float3 colorL = lerp(refracted, reflected, fresnel);

    //����
    float atten = max(1.0 - dot(dist, dist) * 0.001, 0.0);
    colorL += SEA_WATER_COLOR * (p.y - SEA_HEIGHT) * 0.18 * atten;
    
    //���ʔ���
    float s = specular(n, l, eye, 60.0);
    colorL += float3(s, s, s);

    return colorL * seaCol;
}

// tracing
float3 getNormal(float3 p, float eps)
{
    //���C�̓��������g�̈ʒu��
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
    //���C�̎n�_�܂ł̒���
    float tm = 0.0;
    //���C�̏I�_�܂ł̒���
    float tx = 1000.0;

    float3 p = 0;
    
    // 1000.0m��̃w�C�g�}�b�v�̋����i�����̈ʒu����^���̔g�������̈ʒu�ւ̋����j
    float hx = map(ori + dir * tx);

    // 1000.0m������heightmap���v�Z���A0�ȏ�Ȃ琅���ʂ���ŁA�C�Ƀ��C��������Ȃ��̂Ŕ�����
    //�I�_�͊C��艺�ɂ��邱�ƑO��Ȃ̂�
    if (hx > 0.0)
    {
        p = float3(0, 0, 0);
        return tx;
    }

    //�J�����̈ʒu�̃w�C�g�}�b�v�̋����i�����̈ʒu����^���̔g�������̈ʒu�ւ̋����j
    float hm = map(ori + dir * tm);
    float tmid = 0.0;

    // NUM_STEPS = 8
    //���C�̊J�n�n�_����1000.0m��𗼒[�_�Ƃ��Čv�Z
    for (int i = 0; i < NUM_STEPS; i++)
    {
        // hm�i�J�n�_�̃w�C�g�}�b�v�����j��hx�i�I�_�́V�j�̔䗦�ɉ����Ăǂ̈ʒu�̃��C���T���v�����邩�����߂邽�߂́u�d�݁v���v�Z
        float f = hm / (hm - hx);
        //���C�̒�������`��Ԃŏo��
        tmid = lerp(tm, tx, f);
        p = ori + dir * tmid;
        //�w�C�g�}�b�v�̋���
        float hmid = map(p);

        // �T���v�����O�ʒu�̍������}�C�i�X�����̏ꍇ��`hx`, `tx`(�I�_)���X�V����
        //hx�͕��̒l
        if (hmid < 0.0)
        {
            tx = tmid; //�I�_�̃��C�̒���
            hx = hmid; //�I�_�̃w�C�g�}�b�v�̋���
        }
        // �����łȂ��ꍇ�́A`hm`, `tm`�i�n�_�j���X�V����
        //hm�͐��̒l
        else
        {
            tm = tmid; //�n�_�̃��C�̒���
            hm = hmid; //�n�_�̃w�C�g�}�b�v�̋���
        }
    }

    return p;
}

// main
float4 mainImage(float2 fragCoord)
{
    //�𑜓x����uv���W�ɕϊ�
    float2 uv = fragCoord.xy / (IRESO.xy * resoulution);
    uv = uv * 2.0 - 1.0;

    const float3 light = normalize(float3(0.0, 1.0, 0.8));

    // ��]�s��
    float3x3 rotM = fromEuler(seaDirRot * seaDirRotExtend);
    
    //�J�����̈ʒu
    float3 ori = float3(seaCameraPos.z, seaCameraPos.y, seaCameraPos.x);
    //uv���W���g���ĕ����x�N�g�����v�Z
    float3 dir = normalize(float3(uv.x, uv.y, 4.0f));
    //�����x�N�g������]
    dir = mul(rotM, dir);

    // ���C�̓��������ꏊ
    float3 p;
    p = heightMapTracing(ori, dir);

    //�J��������̋���
    float3 dist = p - ori;
    //�@��
    float3 n = getNormal(p, dot(dist, dist) * EPSILON_NRM);

    //�C�Ƌ�̐F
    float3 sky = getSkyColor(dir);
    float3 sea = getSeaColor(p, n, light, dir, dist);

    // �����x�N�g����y���g���ČW���v�Z
    float t = pow(smoothstep(0.0, -0.05, dir.y), 0.3);
    //�C�Ƌ�̐F����`���
    float3 colorL = lerp(sky, sea, t);

    
    //���ŃK���X�t�B���^�[�p�̉摜���g���ėn��\��
    colorL = float4(colorL, 1.0f);
    
    if(isSeaImageEffect)
    {
        colorL *= max(float4(effectTex.Sample(smp, abs(frac(float2(-dist.x - time * 0.1, -dist.z) / 7.0))).rgb * 3.0f, 1.0f),0.01);
    }
    
    // post
    return float4(colorL, 1.0);
}