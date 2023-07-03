#include "Particle.hlsli"

VSOutput main(float4 pos : POSITION, float scale : TEXCOORD, float4 color : COLOR
, float3 rot : ROT)
{
	VSOutput output; // ピクセルシェーダーに渡す値
	output.pos = pos;
	output.scale = scale;
	output.color = color;
	output.rot = rot;
	return output;
}