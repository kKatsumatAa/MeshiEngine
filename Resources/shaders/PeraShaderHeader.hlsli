


cbuffer ConstBufferEffectFlags : register(b0)
{
    //�F
    float4 color;
	//�ڂ���
    unsigned int isMultiTex;
	//�ڂ���
    unsigned int isGaussian;
	//�K�E�V�A���ڂ���
    unsigned int isGaussian2;
    //���W�A���u���[
    unsigned int isRadialBlur;
    //���W�A������
    float radialPow;
	//�G���{�X
    unsigned int isEmboss;
	//�V���[�v�l�X
    unsigned int isSharpness;
	//�~��
    unsigned int isGradation;
	//�A�E�g���C��
    unsigned int isOutLine;
	//�r�l�b�g
    unsigned int isVignette;
	//�M��p��
    unsigned int isBarrelCurve;
    float barrelCurvePow;
	//������
    unsigned int isScanningLine;
	//�O���[�X�P�[��
    unsigned int isGrayScale;
    float grayScalePow;
	//�K���X�t�B���^�[
    unsigned int isGlassFilter;
	//���U�C�N
    unsigned int isMosaic;
	//�l�K�|�W���]
    unsigned int isNega;
	//�l�K�|�W���]
    unsigned int isRGBShift;
    float RGBShiftPow;
	//�u���[��
    unsigned int isBloom;
	//�N���X�t�B���^
    unsigned int isCrossFilter;
    //�u���[���̋���
    float bloomPow;
    //�m�C�Y
    uint isNoise;
    //��ʊE�[�x
    uint isDepthField;
    //�t�H�[�J�X���킹��[�x�l
    float focusDepth;
	//�t�H�[�J�X�̃X���[�X�X�e�b�v�̕��̉���
    float nFocusWidth;
	//�t�H�[�J�X�̃X���[�X�X�e�b�v�̕��̏��
    float focusDiffPow;
	//����
    uint time;
}

cbuffer PostEffect : register(b1)
{
    float4 bkweights[2];
}

//vs����ps�ɑ���
struct Output
{
    float4 svpos : SV_POSITION;
    float2 uv : TEXCOORD;
};

//�ꖇ�ڂ̈��(�ʏ�J���[)
Texture2D<float4> tex0 : register(t0);
//�ꖇ�ڂ̓��(�Ⴄ�J���[�i�@�����܂��j)
Texture2D<float4> tex1 : register(t1);
//�ꖇ�ڂ̎O��(���P�x�p)
Texture2D<float4> tex2 : register(t2);
//�ꖇ�ڂ̎l��(�k���o�b�t�@���P�x)
Texture2D<float4> tex3 : register(t3);
//�ꖇ�ڂ�5��(�k���o�b�t�@���P�x2)
Texture2D<float4> tex4 : register(t4);
//6��(�ʏ�ڂ����i��ʊE�[�x�p�j)
Texture2D<float4> tex5 : register(t5);
//�K���X�t�B���^�[
Texture2D<float4> effectTex : register(t6);
//�[�x�l(float�Ȃ̂�R�l�̂�)
Texture2D<float> depthTex : register(t7);


//�ڂ�������̉摜��Ԃ�
float4 Get5x5GaussianBlur(Texture2D<float4> tex, SamplerState smp, float2 uv, float dx, float dy, float4 rect)
{
    float4 ret = tex.Sample(smp, uv);

    float l1 = -dx, l2 = -2 * dx;
    float r1 = dx, r2 = 2 * dx;
    float u1 = -dy, u2 = -2 * dy;
    float d1 = dy, d2 = 2 * dy;
    l1 = max(uv.x + l1, rect.x) - uv.x;
    l2 = max(uv.x + l2, rect.x) - uv.x;
    r1 = min(uv.x + r1, rect.z - dx) - uv.x;
    r2 = min(uv.x + r2, rect.z - dx) - uv.x;

    u1 = max(uv.y + u1, rect.y) - uv.y;
    u2 = max(uv.y + u2, rect.y) - uv.y;
    d1 = min(uv.y + d1, rect.w - dy) - uv.y;
    d2 = min(uv.y + d2, rect.w - dy) - uv.y;

    return float4((
		tex.Sample(smp, uv + float2(l2, u2)).rgb
		+ tex.Sample(smp, uv + float2(l1, u2)).rgb * 4
		+ tex.Sample(smp, uv + float2(0, u2)).rgb * 6
		+ tex.Sample(smp, uv + float2(r1, u2)).rgb * 4
		+ tex.Sample(smp, uv + float2(r2, u2)).rgb

		+ tex.Sample(smp, uv + float2(l2, u1)).rgb * 4
		+ tex.Sample(smp, uv + float2(l1, u1)).rgb * 16
		+ tex.Sample(smp, uv + float2(0, u1)).rgb * 24
		+ tex.Sample(smp, uv + float2(r1, u1)).rgb * 16
		+ tex.Sample(smp, uv + float2(r2, u1)).rgb * 4

		+ tex.Sample(smp, uv + float2(l2, 0)).rgb * 6
		+ tex.Sample(smp, uv + float2(l1, 0)).rgb * 24
		+ ret.rgb * 36
		+ tex.Sample(smp, uv + float2(r1, 0)).rgb * 24
		+ tex.Sample(smp, uv + float2(r2, 0)).rgb * 6

		+ tex.Sample(smp, uv + float2(l2, d1)).rgb * 4
		+ tex.Sample(smp, uv + float2(l1, d1)).rgb * 16
		+ tex.Sample(smp, uv + float2(0, d1)).rgb * 24
		+ tex.Sample(smp, uv + float2(r1, d1)).rgb * 16
		+ tex.Sample(smp, uv + float2(r2, d1)).rgb * 4

		+ tex.Sample(smp, uv + float2(l2, d2)).rgb
		+ tex.Sample(smp, uv + float2(l1, d2)).rgb * 4
		+ tex.Sample(smp, uv + float2(0, d2)).rgb * 6
		+ tex.Sample(smp, uv + float2(r1, d2)).rgb * 4
		+ tex.Sample(smp, uv + float2(r2, d2)).rgb
		) / 256.0f, ret.a);
}

//unity�̕���
float4 GaussianTmp(float2 drawUV, float2 pickUV, float sigma)
{
    float d = distance(drawUV, pickUV);
    return exp(-(d * d) / (2.0 * sigma * sigma));
}

float4 Gaussian2(Texture2D<float4> tex, SamplerState smp, float2 uv)
{
    float totalWeight = 0, sigma = 0.03, stepWidth = 0.005;
    float4 col = float4(0, 0, 0, 0);

    for (float py = -sigma * 2; py <= sigma * 2; py += stepWidth)
    {
        for (float px = -sigma * 2; px <= sigma * 2; px += stepWidth)
        {
            float2 pickUV = uv + float2(px, py);
            float weight = GaussianTmp(uv, pickUV, sigma);
            col += tex.Sample(smp, pickUV) * weight; //�d�݂�F�ɂ�����
            totalWeight += weight; //�d�݂̍��v�l���v�Z
        }
    }

    col.rgb = col.rgb / totalWeight; //�|�����d�ݕ����ʂ��犄��
    col.a = 1;
    return col;
}

float4 GaussianAngle(Texture2D<float4> tex, SamplerState smp, float angle, float2 uv)
{
    float totalWeight = 0, sigma = 0.005, stepWidth = 0.001;
    float color = float4(0, 0, 0, 0);
    float2 pickUV = float2(0, 0); //�F���擾������W
    float pickRange = 0.06; //�K�E�X�֐����ł����V�O�}
    float angleDeg = angle;
    float angleRad = angleDeg * 3.1415 / 180; //�ڂ����p�x

    for (float j = -pickRange * 2; j <= pickRange * 2; j += 0.005)
    {
        float x = cos(angleRad) * j; //�p�x������W�w��
        float y = sin(angleRad) * j;
        pickUV = uv + float2(x, y);

        float weight = GaussianTmp(uv, pickUV, pickRange); //����̃K�E�X�֐��Ōv�Z
        color += tex.Sample(smp, pickUV) * weight; //�擾����F��weight��������
        totalWeight += weight; //�|����weight�̍��v�l���T����
    }
    color = color / totalWeight; //�������킹���F��weight�̍��v�l�Ŋ���
    return color;
}

float4 GetRadialBlur(Texture2D<float4> tex, SamplerState smp, float2 center, float2 uv, float2 pixLength, float m_BlurPower)
{
    float4 Color[10];
   
   //�u���[�̒��S�ʒu �� ���݂̃e�N�Z���ʒu
    float2 dir = center - float2(uv.r * center.r * 2.0,
    uv.g * center.g * 2.0);
   
   //�������v�Z����
    float len = length(dir);
   
   //�����x�N�g���̐��K�����A�P�e�N�Z�����̒����ƂȂ�����x�N�g�����v�Z����
    dir = normalize(dir) * pixLength;
   
   //m_BlurPower �Ń{�P��𒲐�����
   //������ώZ���邱�Ƃɂ��A�����̒��S�ʒu�ɋ߂��قǃu���[�̉e�����������Ȃ�悤�ɂ���
    dir *= m_BlurPower * len;

   //��������
    Color[0] = tex.Sample(smp, uv) * 0.19f;
    Color[1] = tex.Sample(smp, uv + dir) * 0.17f;
    Color[2] = tex.Sample(smp, uv + dir * 2.0f) * 0.15f;
    Color[3] = tex.Sample(smp, uv + dir * 3.0f) * 0.13f;
    Color[4] = tex.Sample(smp, uv + dir * 4.0f) * 0.11f;
    Color[5] = tex.Sample(smp, uv + dir * 5.0f) * 0.09f;
    Color[6] = tex.Sample(smp, uv + dir * 6.0f) * 0.07f;
    Color[7] = tex.Sample(smp, uv + dir * 7.0f) * 0.05f;
    Color[8] = tex.Sample(smp, uv + dir * 8.0f) * 0.03f;
    Color[9] = tex.Sample(smp, uv + dir * 9.0f) * 0.01f;
   
    return Color[0] + Color[1] + Color[2] + Color[3] + Color[4] + Color[5] + Color[6] + Color[7] + Color[8] + Color[9];
}

//�����_��
float random(float2 p, float extend = 1.0f);
float random(float2 p, float extend)
{
    return frac(sin(dot(p, float2(12.9898f, 78.233f))) * (43758.5453f + extend));
}

//�m�C�Y
float noise(float2 st, float extend = 1.0f);
float noise(float2 st, float extend)
{
    float2 p = floor(st);
    return random(p, extend); //
}