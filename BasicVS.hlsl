#include "Basic.hlsli"

VSOutput main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
	//VSOutput output;//ピクセルシェーダーに渡す値(hlsliの構造体)
	////output.svpos = pos;
	//output.svpos = mul(mat, pos);//座標に行列を乗算
	//output.normal = normal;
	//output.uv = uv;

	//ランバート
	float3 lightdir = float3(1, -1, 1);
	lightdir = normalize(lightdir);
	float3 lightcolor = float3(1, 1, 1);
	VSOutput output;
	output.svpos = mul(mat, pos);

	output.color.rgb = dot(-lightdir, normal) * 1.5f * lightcolor/*+(0.1,0.1,0.1)*/;
	output.color.a = 1.0f;
	output.uv = uv;

	return output;
}