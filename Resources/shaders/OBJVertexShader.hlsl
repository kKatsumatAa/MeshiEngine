
#include "OBJShaderHeader.hlsli"

VSOutput main(float4 pos : POSITION, float3 normal : NORMAL,
	float2 uv : TEXCOORD)
{
	//右、下、奥の方向を向いたライト
	float3 lightdir = float3(1, -1, 1);
	lightdir = normalize(lightdir);
	//ライトの色（白）
	float3 lightcolor = float3(1, 1, 1);
	//ピクセルシェーダに渡す値
	VSOutput output;//ピクセルシェーダに渡す値
	output.svpos = mul(mat, pos);
	//Lambert反射の計算
	output.color.rgb = dot(-lightdir, normal) * m_diffuse * lightcolor;
	output.color.a = m_alpha;
	output.uv = uv;
	return output;
}