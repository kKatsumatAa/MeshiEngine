#include"PeraShaderHeader.hlsli"

Texture2D<float4> tex : register(t0);

SamplerState smp : register(s0);

float4 PS2(Output input) : SV_TARGET
{
	// シェーディングによる色で描画
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
			ret += bkweights[i >> 2][i % 4] * tex.Sample(smp, input.uv + float2(0, dy * i));
			ret += bkweights[i >> 2][i % 4] * tex.Sample(smp, input.uv + float2(0, -dy * i));
		}

		isEffect = true;
	}

	if (isEffect)
	{
		return ret;
	}

	return RGBA;
}