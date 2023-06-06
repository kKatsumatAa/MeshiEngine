#include "Sprite.hlsli"

VSOutput main(float4 pos : POSITION, float2 uv : TEXCOORD)
{
	//ピクセルシェーダに渡す値
	VSOutput output;//ピクセルシェーダに渡す値
	output.svpos = mul(mul(viewproj, world), pos);
	output.uv = uv;

	return output;
}