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
	output.col2 = output.col;
	//三つ目
	float y = dot(float3(0.299f, 0.587f, 0.114f), output.col);
	output.highLumi = y > 0.99f ? output.col : 0.0f;

	return output;
}