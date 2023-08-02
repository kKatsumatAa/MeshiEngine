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

	return output;
}