#include "Sprite.hlsli"

Texture2D<float4> tex : register(t0); //0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);      //0番スロットに設定されたサンプラー


float4 main(VSOutput input) : SV_TARGET
{
	//テクスチャ
	float4 texcolor = float4(tex.Sample(smp, input.uv));

	/*if (texcolor.a != 0&& texcolor.r == 0&& texcolor.g == 0&& texcolor.b == 0)
	{
		return color;
	}
	else*/
	{
		return texcolor * color;
	}
}