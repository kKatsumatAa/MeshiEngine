#include "Basic.hlsli"

VSOutput main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
	//法線にワールド行列によるスケーリング・回転を適用
	float4 wnormal = normalize(mul(world, float4(normal, 0)));
	float4 wpos = mul(world, pos);

	//ピクセルシェーダに渡す値
	VSOutput output;//ピクセルシェーダに渡す値
	output.svpos = mul(mul(viewproj, world), pos);

	//環境反射光
	float3 ambient = 0.01f;
	//拡散反射光
	float3 diffuse = dot(lightv, wnormal.xyz) * 1.0f;
	//光沢度
	const float shininess = 4.0f;
	//頂点から視点への方向ベクトル
	float3 eyedir = normalize(cameraPos - wpos.xyz);
	//反射光ベクトル//一個目のlightdirは-しない
	float3 reflect = normalize(-lightv + 2 * dot(lightv, wnormal.xyz) * wnormal.xyz);
	//鏡面反射光（saturate->0~1へのクランプ）
	float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * 0.1f;

	//全て加算する
	output.color.rgb = (ambient + diffuse + specular) * lightcolor;
	output.color.a = 1.0f;
	output.uv = uv;
	return output;

	return output;
}