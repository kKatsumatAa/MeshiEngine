#include "Basic.hlsli"

VSOutput main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
	//VSOutput output;//ピクセルシェーダーに渡す値(hlsliの構造体)
	////output.svpos = pos;
	//output.svpos = mul(mat, pos);//座標に行列を乗算
	//output.normal = normal;
	//output.uv = uv;

//ランバート
	float3 lightdir = normalize(float3(1, -1, 1));
	lightdir = normalize(lightdir);
	float3 lightcolor = float3(1, 1, 1);
	VSOutput output;
	output.svpos = mul(mat, pos);
	 //ランバートを入れとく
	float3 diffuse = saturate(dot(-lightdir, normal));

//フォン
	//環境光
	float3 ambient = float3(0.1f, 0.1f, 0.1f);

	const float3 eye = float3(0, 0, -20.0f);
	const float shine = 4.0f;
	float3 eyedir = normalize(eye - pos.xyz);
	 //反射光ベクトル
	float3 reflect = normalize( + 2.0f * dot(lightdir, normal) * normal - lightdir);
	float3 specular = pow(saturate(dot(reflect, eyedir)), shine);
	//if (specular.x < 0.0f) specular.x = 0.0f;
	//if (specular.y < 0.0f) specular.y = 0.0f;
	//if (specular.z < 0.0f) specular.z = 0.0f;

	output.color.rgb = ((ambient + diffuse + specular) * lightcolor);
	output.color.a = 1.0f;
	output.uv = uv;

	return output;
}