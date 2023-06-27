#include "Particle.hlsli"

Texture2D<float4> tex : register(t0);  // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);      // 0番スロットに設定されたサンプラー

struct PSOutput
{
	float4 col : SV_TARGET0;//通常のレンダリング
	float4 col2 : SV_TARGET1;//法線
	float4 highLumi : SV_TARGET2;//高輝度
};

PSOutput main(GSOutput input)
{
	PSOutput output;

	//一つ目
	output.col = tex.Sample(smp, input.uv) * input.color;
	//二つ目
	//output.col2 = float4(1.0f - input.color.r, 1.0f - input.color.g, 1.0f - input.color.b, 1.0f);
	//三つ目
	//output.highLumi = tex.Sample(smp, input.uv) * input.color;

	return output;
}