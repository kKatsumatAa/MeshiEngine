#include "Sprite.hlsli"

Texture2D<float4> tex : register(t0); //0番スロットに設定されたテクスチャ
Texture2D<float4> tex2 : register(t1); //1番スロットに設定されたテクスチャ
SamplerState smp : register(s0);      //0番スロットに設定されたサンプラー


float4 main(VSOutput input) : SV_TARGET
{
	//テクスチャ
	float4 texcolor = float4(tex.Sample(smp, input.uv));
	
	{
		return texcolor * color;
	}
}