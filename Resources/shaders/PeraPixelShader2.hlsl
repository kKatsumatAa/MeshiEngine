#include"PeraShaderHeader.hlsli"

SamplerState smp : register(s0);

float4 PS2(Output input) : SV_TARGET
{
	// �V�F�[�f�B���O�ɂ��F�ŕ`��
    float4 RGBA = tex0.Sample(smp, input.uv);
    float3 RGB = RGBA.rgb;
    float A = RGBA.a;

    float w, h, level;

    tex0.GetDimensions(0, w, h, level);

    float dx = 1.0f / w;
    float dy = 1.0f / h;

    float4 ret = float4(0, 0, 0, 0);
    float4 col = tex0.Sample(smp, input.uv);

    bool isEffect = false;

    if (isGaussian2)
    {
        ret += bkweights[0] * col;
        for (int i = 1; i < 8; ++i)
        {
            ret += bkweights[i >> 2][i % 4] * tex0.Sample(smp, clamp(input.uv + float2(0, dy * i), 0, 1.0f));
            ret += bkweights[i >> 2][i % 4] * tex0.Sample(smp, clamp(input.uv + float2(0, -dy * i), 0, 1.0f));
        }

        isEffect = true;
    }
	
	//���W�A��
    if (isRadialBlur)
    {
        ret += GetRadialBlur(tex0, smp, float2(w, h) / 2.0f, input.uv, 0.0005f * radialPow, 0.021f * radialPow);

        isEffect = true;
    }

	//�@���}�b�v
    if (isGlassFilter)
    {
        float2 nmTex = effectTex.Sample(smp, input.uv).xy;
        nmTex = nmTex * 2.0f - 1.0f;

		//nmTex�͈̔͂�-1�`1�����A��1���e�N�X�`��1����
		//�傫���ł���-1�`1�ł͘c�݂����邽��0.1����Z���Ă���
        ret = tex0.Sample(smp, input.uv + nmTex * 0.1f);
        
        isEffect = true;
    }
    
     //�m�C�Y
    if (isNoise)
    {
        float c = noise(input.uv * 256.0f, time);
        return float4(c, c, c, 1.0f);
    }
   
        
    //�[�x�l
    if (isDepthField)
    {
       // �[�x�l
        float depth = depthTex.Sample(smp, input.uv).r;
       // �t�H�[�J�X�����Ă�
        float inFocus = 1.0f - smoothstep(0, nFocusWidth, abs(depth - focusDepth));
        //�t�H�[�J�X����̐[�x�̍��𑪂�
        float depthDiff = abs(inFocus - depth);
        //�߂��l�ł������o��悤��pow
        depthDiff = pow(depthDiff, focusDiffPow);
        
        float2 uvSize = float2(1.0f, 0.5);
        float2 uvOfst = float2(0, 0);
        //�ő�1.0f ��depth�l�̍��𕡐��̏k���o�b�t�@�ƃu�����h�ł���悤�ɔ{
        const int SHRINK_EXTEND = 6;
        float t = depthDiff * SHRINK_EXTEND;
        
        float no;
        //t�̐���������no�ցA ����������t�֕�����

        t = modf(t, no);
        
        float4 retColor[2];
        
        retColor[0] = tex0.Sample(smp, input.uv); //�ʏ�e�N�X�`��
        //�����������O

        if (no == 0.0f)
        {
            retColor[1] = Get5x5GaussianBlur(tex5, smp, input.uv * uvSize + uvOfst, dx, dy, float4(uvOfst, uvOfst + uvSize));
        }
        //���������������
        else
        {
            for (int i = 1; i <= SHRINK_EXTEND; ++i)
            {
                if (i - no < 0)
                {
                    continue;
                }
                retColor[i - no] = Get5x5GaussianBlur(tex5, smp, input.uv * uvSize + uvOfst, dx, dy, float4(uvOfst, uvOfst + uvSize));
                uvOfst.y += uvSize.y;
                uvSize *= 0.5f;
                if (i - no > 1.0f)
                {
                    break;
                }
            }
        }
        ret = lerp(retColor[0], retColor[1], t);
        
        isEffect = true;
    }


	//�u���[��
    if (isBloom)
    {
        float w, h, level;

        tex3.GetDimensions(0, w, h, level);

        float dx = 1.0f / w;
        float dy = 1.0f / h;

        float4 bloomAccum = float4(0, 0, 0, 0);
        float2 uvSize = float2(1.0, 0.5);
        float2 uvOfst = float2(0, 0);

		//i�̏����ς���Ƌ������ς��i8�ɂ���ƍׂ������Ă��������Ȃ�j
        for (int i = 0; i < 6; ++i)
        {
            bloomAccum += Get5x5GaussianBlur(
				tex3, smp, input.uv * uvSize + uvOfst, dx, dy, float4(uvOfst, uvOfst + uvSize));
            uvOfst.y += uvSize.y;
            uvSize *= 0.5;
        }

		//���̉摜�Ƃڂ��������P�x�̕����𑫂�
        return tex0.Sample(smp, input.uv) //�ʏ�e�N�X�`��
			+ saturate(bloomAccum) / bloomPow; //�k���ڂ����ς�(/ �ŋ�������)
    }

	//�N���X�t�B���^
    if (isCrossFilter)
    {
        float w, h, level;

        tex3.GetDimensions(0, w, h, level);

        float dx = 1.0f / w;
        float dy = 1.0f / h;

        float4 bloomAccum[2] = { float4(0, 0, 0, 0), float4(0, 0, 0, 0) };
        float2 uvSize = float2(1.0, 0.5);
        float2 uvOfst = float2(0, 0);

		//i�̏����ς���Ƌ������ς��i8�ɂ���ƍׂ������Ă��������Ȃ�j
        for (int i = 0; i < 2; ++i)
        {
            bloomAccum[0] += Get5x5GaussianBlur(
				tex3, smp, input.uv * uvSize + uvOfst, dx, dy, float4(uvOfst, uvOfst + uvSize));
            uvOfst.y += uvSize.y;
            uvSize *= 0.5;
        }
        uvSize = float2(1.0, 0.5);
        uvOfst = float2(0, 0);
        for (int i = 0; i < 2; ++i)
        {
            bloomAccum[1] += Get5x5GaussianBlur(
				tex4, smp, input.uv * uvSize + uvOfst, dx, dy, float4(uvOfst, uvOfst + uvSize));
            uvOfst.y += uvSize.y;
            uvSize *= 0.5;
        }

		//���̉摜�Ƃڂ��������P�x�̕����𑫂�
        return tex0.Sample(smp, input.uv) //�ʏ�e�N�X�`��
			+ saturate(bloomAccum[0]) / bloomPow //�k���ڂ����ς�(/ �ŋ�������)
			+ saturate(bloomAccum[1]) / bloomPow; //�k���ڂ����ς�(/ �ŋ�������)
    }
    
    if (isEffect)
    {
        return ret;
    }

    return RGBA;
}