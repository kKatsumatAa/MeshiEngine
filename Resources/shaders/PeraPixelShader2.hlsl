#include"PeraShaderHeader.hlsli"

Texture2D<float4> tex : register(t0);
//�K���X�t�B���^�[
Texture2D<float4> effectTex : register(t1);

SamplerState smp : register(s0);

float4 PS2(Output input) : SV_TARGET
{
	// �V�F�[�f�B���O�ɂ��F�ŕ`��
	float4 RGBA = tex.Sample(smp, input.uv);
	float3 RGB = RGBA.rgb;
	float  A = RGBA.a;

	float w,h,level;

	tex.GetDimensions(0,w,h,level);

	float dy = 1.0f / h;
	float4 ret = float4(0,0,0,0);
	float4 col = tex.Sample(smp,input.uv);

	bool isEffect = false;


	if (isGaussian2)
	{
		ret += bkweights[0] * col;
		for (int i = 1; i < 8; ++i) {
			ret += bkweights[i >> 2][i % 4] * tex.Sample(smp, clamp(input.uv + float2(0, dy * i),0,1.0f));
			ret += bkweights[i >> 2][i % 4] * tex.Sample(smp, clamp(input.uv + float2(0, -dy * i),0,1.0f));
		}

		isEffect = true;
	}

	//�@���}�b�v
	if(isGlassFilter)
	{
		float2 nmTex = effectTex.Sample(smp, input.uv).xy;
		nmTex = nmTex * 2.0f - 1.0f;

		//nmTex�͈̔͂�-1�`1�����A��1���e�N�X�`��1����
		//�傫���ł���-1�`1�ł͘c�݂����邽��0.1����Z���Ă���
		return tex.Sample(smp, clamp(input.uv + nmTex * 0.1f,0.0f,1.0f));
	}


	if (isEffect)
	{
		return ret;
	}

	return RGBA;
}