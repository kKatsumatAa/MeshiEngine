#include"PeraShaderHeader.hlsli"

Texture2D<float4> tex : register(t0);
//ガラスフィルター
Texture2D<float4> effectTex : register(t1);

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
			ret += bkweights[i >> 2][i % 4] * tex.Sample(smp, clamp(input.uv + float2(0, dy * i),0,1.0f));
			ret += bkweights[i >> 2][i % 4] * tex.Sample(smp, clamp(input.uv + float2(0, -dy * i),0,1.0f));
		}

		isEffect = true;
	}

	//法線マップ
	if(isGlassFilter)
	{
		float2 nmTex = effectTex.Sample(smp, input.uv).xy;
		nmTex = nmTex * 2.0f - 1.0f;

		//nmTexの範囲は-1～1だが、幅1がテクスチャ1枚の
		//大きさであり-1～1では歪みすぎるため0.1を乗算している
		return tex.Sample(smp, clamp(input.uv + nmTex * 0.1f,0.0f,1.0f));
	}


	if (isEffect)
	{
		return ret;
	}

	return RGBA;
}