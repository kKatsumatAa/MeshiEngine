#include "Particle.hlsli"

Texture2D<float4> tex : register(t0);  // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);      // 0番スロットに設定されたサンプラー

struct PSOutput
{
	float4 target0 : SV_TARGET0;
	float4 target1 : SV_TARGET1;
};

PSOutput main(GSOutput input)
{
	PSOutput output;

	output.target0 = tex.Sample(smp, input.uv) * input.color;
	output.target1 = output.target0;

	return output;
}