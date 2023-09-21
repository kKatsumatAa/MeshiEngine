#include"PeraShaderHeader.hlsli"

SamplerState smp : register(s0);

//���P�x�ƒʏ�
struct PSOutput
{
    float4 color : SV_TARGET0; //�ʏ�
    float4 color2 : SV_TARGET1; //�F�Ⴂ
    float4 highLumi : SV_TARGET2; //���P�x
};

PSOutput PS(Output input) : SV_TARGET
{
    PSOutput output;

// �V�F�[�f�B���O�ɂ��F�ŕ`��
    float4 RGBA = tex0.Sample(smp, input.uv);
    float3 RGB = RGBA.rgb;
    float A = RGBA.a;

    float w, h, levels;
//�~�b�v�}�b�v�A�����A�c���A�~�b�v�}�b�v���x��
    tex0.GetDimensions(0, w, h, levels);

    float dx = 1.0f / w;
    float dy = 1.0f / h;
    float4 ret = float4(0, 0, 0, 0);
    bool isEffect = false;

//�V���[�v�l�X
    if (isSharpness == true)
    {
        ret += tex0.Sample(smp, input.uv + float2(-2 * dx, -2 * dy)) * 0; // ���� 
        ret += tex0.Sample(smp, input.uv + float2(0, -2 * dy)); // �� 
        ret += tex0.Sample(smp, input.uv + float2(2 * dx, -2 * dy)) * 0; // �E �� 
        ret += tex0.Sample(smp, input.uv + float2(-2 * dx, 0)); // �� 
        ret += tex0.Sample(smp, input.uv) * 5.0f; // ���� 
        ret += tex0.Sample(smp, input.uv + float2(2 * dx, 0)); // �E
        ret += tex0.Sample(smp, input.uv + float2(-2 * dx, 2 * dy)) * 0; // ���� 
        ret += tex0.Sample(smp, input.uv + float2(0, 2 * dy)); // �� 
        ret += tex0.Sample(smp, input.uv + float2(2 * dx, 2 * dy)) * 0; // �E ��

        isEffect = true;
    }

//�֊s
    if (isOutLine == true)
    {
        ret += tex0.Sample(smp, input.uv + float2(0, -2 * dy)) * -1; // ��
        ret += tex0.Sample(smp, input.uv + float2(-2 * dx, 0)) * -1; // ��
        ret += tex0.Sample(smp, input.uv) * 4; // ���� 
        ret += tex0.Sample(smp, input.uv + float2(2 * dx, 0)) * -1; // �E 
        ret += tex0.Sample(smp, input.uv + float2(0, 2 * dy)) * -1; // �� 
	// ���] 
        float Y = dot(ret.rgb * RGBA.rgb, float3(0.299, 0.587, 0.114));
        Y = pow(1.0f - Y, 10.0f);
        Y = step(0.2, Y);
        ret = float4(Y, Y, Y, A);

        isEffect = true;
    }

//�K�E�V�A��
    if (isGaussian == true)
    {
        float dx = 2.0f / w;
        float dy = 2.0f / h;
	// �� �� �s�N�Z�� �� ���S �� �c�� 5 �� ���� �� �Ȃ� �悤 ���Z ���� 
	// �� ��i 
        ret += Gaussian2(tex0, smp, input.uv);

        isEffect = true;
    }

//�K�E�V�A���Q
    if (isGaussian2 == true)
    {
        ret += bkweights[0] * RGBA;
        for (int i = 1; i < 8; ++i)
        {
            ret += bkweights[i >> 2][i % 4] * tex0.Sample(smp, clamp(input.uv + float2(i * dx, 0), 0, 1.0f));
            ret += bkweights[i >> 2][i % 4] * tex0.Sample(smp, clamp(input.uv + float2(-i * dx, 0), 0, 1.0f));
        }

        isEffect = true;
    }

//�~��
    if (isGradation)
    {
        ret = float4(RGB - fmod(RGB, 0.25f), A);

        isEffect = true;
    }

//�r�l�b�g
    if (isVignette)
    {
        float2 samplePoint = input.uv;
        float4 Tex = tex0.Sample(smp, input.uv);
        float vignette = length(float2(0.5f, 0.5f) - input.uv);
        vignette = clamp(vignette - 0.45f, 0, 1);
        Tex.rgb -= vignette;
        ret = Tex;

        isEffect = true;
    }

//������
    if (isScanningLine)
    {
        float extend = 0.1f;
        float2 samplePoint = input.uv;
        float4 Tex = tex0.Sample(smp, samplePoint);
        float sinv = sin(input.uv.y * 2 + time * extend * -0.1);
        float steped = step(0.99, sinv * sinv);
        Tex.rgb -= (1 - steped) * abs(sin(input.uv.y * 50.0 + time * extend * 1.0)) * 0.05;
        Tex.rgb -= (1 - steped) * abs(sin(input.uv.y * 100.0 - time * extend * 2.0)) * 0.08;
        Tex.rgb += steped * 0.1;
        ret = Tex;

        isEffect = true;
    }

//�O���[
    if (isGrayScale)
    {
	// �e�N�X�`�������f�l�擾
        float4 tcolor = RGBA;

        float y, u, v;
        y = tcolor.x * 0.199 + tcolor.y * 0.487 + tcolor.z * 0.014;

		// �O���[�X�P�[���ϊ�
        tcolor.x = lerp(tcolor.r, y, grayScalePow);
        tcolor.y = lerp(tcolor.g, y, grayScalePow);
        tcolor.z = lerp(tcolor.b, y, grayScalePow);

		// �o�͂���s�N�Z���F
        ret = float4(tcolor.rgb, 1.0f);

        isEffect = true;
    }

    //�G���{�X
    if (isEmboss == true)
    {
        ret += tex0.Sample(smp, input.uv + float2(-2 * dx, -2 * dy)) * 2; // ����
        ret += tex0.Sample(smp, input.uv + float2(0, -2 * dy)); // ��
        ret += tex0.Sample(smp, input.uv + float2(2 * dx, -2 * dy)) * 0; // �E ��
        ret += tex0.Sample(smp, input.uv + float2(-2 * dx, 0)); // ��
        ret += tex0.Sample(smp, input.uv); // ����
        ret += tex0.Sample(smp, input.uv + float2(2 * dx, 0)) * -1; // �E
        ret += tex0.Sample(smp, input.uv + float2(-2 * dx, 2 * dy)) * 0; // ����
        ret += tex0.Sample(smp, input.uv + float2(0, 2 * dy)) * -1; // �� 
        ret += tex0.Sample(smp, input.uv + float2(2 * dx, 2 * dy)) * -2; // �E �� 

        isEffect = true;
    }
    
    //�M��
    if (isBarrelCurve)
    {
        float2 samplePoint = input.uv;
        samplePoint -= float2(0.5, 0.5);
        float distPower = pow(length(samplePoint), 0.1 * barrelCurvePow);
        samplePoint *= float2(distPower, distPower);
        samplePoint += float2(0.5, 0.5);
        float4 Tex = tex0.Sample(smp, samplePoint);
        ret = Tex;

        isEffect = true;
    }
    
//���U�C�N
    if (isMosaic)
    {
        float density = 50.0f;
        ret = tex0.Sample(smp, floor(input.uv * density) / density);

        isEffect = true;
    }

//�l�K�|�W
    if (isNega)
    {
        ret = float4(1.0f - RGBA.r, 1.0f - RGBA.g, 1.0f - RGBA.b, 1.0f);

        isEffect = true;
    }

//RGB���炵
    if (isRGBShift)
    {
        float shift = RGBShiftPow;
        float r = tex0.Sample(smp, input.uv + float2(-shift, 0)).r;
        float g = tex0.Sample(smp, input.uv + float2(0, 0)).g;
        float b = tex0.Sample(smp, input.uv + float2(shift, 0)).b;

        ret = float4(r, g, b, 1.0f);

        isEffect = true;
    }

    if (isMultiTex)
    {
        float dx = 1.0f / w;
        float dy = 1.0f / h;
	// �� �� �s�N�Z�� �� ���S �� �c�� 5 �� ���� �� �Ȃ� �悤 ���Z ���� 
	// �� ��i 
        ret += tex0.Sample(smp, clamp(input.uv + float2(-2 * dx, 2 * dy), 0, 1.0f)) * 1 / 256;
        ret += tex0.Sample(smp, clamp(input.uv + float2(-1 * dx, 2 * dy), 0, 1.0f)) * 4 / 256;
        ret += tex0.Sample(smp, clamp(input.uv + float2(0 * dx, 2 * dy), 0, 1.0f)) * 6 / 256;
        ret += tex0.Sample(smp, clamp(input.uv + float2(1 * dx, 2 * dy), 0, 1.0f)) * 4 / 256;
        ret += tex0.Sample(smp, clamp(input.uv + float2(2 * dx, 2 * dy), 0, 1.0f)) * 1 / 256;
	// 1 �� ��i 
        ret += tex0.Sample(smp, clamp(input.uv + float2(-2 * dx, 1 * dy), 0, 1.0f)) * 4 / 256;
        ret += tex0.Sample(smp, clamp(input.uv + float2(-1 * dx, 1 * dy), 0, 1.0f)) * 16 / 256;
        ret += tex0.Sample(smp, clamp(input.uv + float2(0 * dx, 1 * dy), 0, 1.0f)) * 24 / 256;
        ret += tex0.Sample(smp, clamp(input.uv + float2(1 * dx, 1 * dy), 0, 1.0f)) * 16 / 256;
        ret += tex0.Sample(smp, clamp(input.uv + float2(2 * dx, 1 * dy), 0, 1.0f)) * 4 / 256;
	// ���i 
        ret += tex0.Sample(smp, clamp(input.uv + float2(-2 * dx, 0 * dy), 0, 1.0f)) * 6 / 256;
        ret += tex0.Sample(smp, clamp(input.uv + float2(-1 * dx, 0 * dy), 0, 1.0f)) * 24 / 256;
        ret += tex0.Sample(smp, clamp(input.uv + float2(0 * dx, 0 * dy), 0, 1.0f)) * 36 / 256;
        ret += tex0.Sample(smp, clamp(input.uv + float2(1 * dx, 0 * dy), 0, 1.0f)) * 24 / 256;
        ret += tex0.Sample(smp, clamp(input.uv + float2(2 * dx, 0 * dy), 0, 1.0f)) * 6 / 256;
	// 1 �� ���i 
        ret += tex0.Sample(smp, clamp(input.uv + float2(-2 * dx, -1 * dy), 0, 1.0f)) * 4 / 256;
        ret += tex0.Sample(smp, clamp(input.uv + float2(-1 * dx, -1 * dy), 0, 1.0f)) * 16 / 256;
        ret += tex0.Sample(smp, clamp(input.uv + float2(0 * dx, -1 * dy), 0, 1.0f)) * 24 / 256;
        ret += tex0.Sample(smp, clamp(input.uv + float2(1 * dx, -1 * dy), 0, 1.0f)) * 16 / 256;
        ret += tex0.Sample(smp, clamp(input.uv + float2(2 * dx, -1 * dy), 0, 1.0f)) * 4 / 256;
	// �� ���i 
        ret += tex0.Sample(smp, clamp(input.uv + float2(-2 * dx, -2 * dy), 0, 1.0f)) * 1 / 256;
        ret += tex0.Sample(smp, clamp(input.uv + float2(-1 * dx, -2 * dy), 0, 1.0f)) * 4 / 256;
        ret += tex0.Sample(smp, clamp(input.uv + float2(0 * dx, -2 * dy), 0, 1.0f)) * 6 / 256;
        ret += tex0.Sample(smp, clamp(input.uv + float2(1 * dx, -2 * dy), 0, 1.0f)) * 4 / 256;
        ret += tex0.Sample(smp, clamp(input.uv + float2(2 * dx, -2 * dy), 0, 1.0f)) * 1 / 256;

        isEffect = true;
    }

    if (isEffect)
    {
        output.color = ret * color;
    }
    else
    {
        output.color = RGBA * color;
    }

//�u���[��
    if (isBloom || isCrossFilter)
    {
        float4 col = tex0.Sample(smp, input.uv);
        float grayScale = col.r * 0.299 + col.g * 0.587 + col.b * 0.144;
        float extract = smoothstep(0.6, 0.9, grayScale);

        output.highLumi = col * extract;

	//�h�b�g�t�B���^
        if (isCrossFilter)
        {
            float st = input.uv / w * 20;
            st = frac(st * float2(w, h));
            float l = distance(st, float2(0.5, 0.5));
            output.highLumi = output.highLumi * float4(1, 1, 1, 1) * 1 - step(0.3, l);
        }
    }

//�F�Ⴂ
    output.color2 = float4(1.0f - RGBA.r, 1.0f - RGBA.g, 1.0f - RGBA.b, 1.0f);

    return output;
}