#include "Basic.hlsli"

VSOutput main(float4 pos : POSITION, float2 uv : TEXCOORD)
{
	VSOutput output;//ピクセルシェーダーに渡す値(hlsliの構造体)
	output.svpos = pos;
	output.uv = uv;

	return output;
}