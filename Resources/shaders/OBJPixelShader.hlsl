
#include "OBJShaderHeader.hlsli"

Texture2D<float4> tex : register(t0); //0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);      //0番スロットに設定されたサンプラー

struct PSOutput
{
	float4 col : SV_TARGET0;//通常のレンダリング
	float4 col2 : SV_TARGET1;//色２
	float4 highLumi : SV_TARGET2;//高輝度
};

PSOutput main(VSOutput input)
{
	PSOutput output;

	// テクスチャマッピング
	float4 texcolor = tex.Sample(smp, input.uv);

	// 光沢度
	const float shininess = 4.0f;
	// 頂点から視点への方向ベクトル
	float3 eyedir = normalize(cameraPos - input.worldpos.xyz);

	// 環境反射光
	float3 ambient = float3(ambientColor * m_ambient);

	// シェーディングによる色
	float4 shadecolor = { 0,0,0, m_alpha };

	//平行光源
	for (int i = 0; i < DIRLIGHT_NUM; i++) {
		if (dirLights[i].active) {
			// ライトに向かうベクトルと法線の内積
			float3 dotlightnormal = dot(dirLights[i].lightv, input.normal);
			// 反射光ベクトル
			float3 reflect = normalize(-dirLights[i].lightv + 2 * dotlightnormal * input.normal);
			float3 specular = pow(saturate(dot(reflect, eyedir)), shininess);
			//トゥーン
			if (isToon) {
				specular = smoothstep(0.5f, 0.55f, specular);
				dotlightnormal = (1.0f - specular) * smoothstep(0.5f, 0.55f, dotlightnormal);
				ambient = (1.0f - dotlightnormal - specular) * ambient;
			}

			// 拡散反射光
			float3 diffuse = dotlightnormal * m_diffuse * diffuseColor;
			// 鏡面反射光
			specular = specular * m_specular * specularColor;

			// 全て加算する
			shadecolor.rgb += (diffuse + specular) * dirLights[i].lightcolor;
		}
	}

	//点光源
	for (int i = 0; i < POINTLIGHT_NUM; i++) {
		if (pointLights[i].active) {
			//ライトへのベクトル
			float3 lightv = pointLights[i].lightpos - input.worldpos.xyz;
			//ベクトルの長さ
			float d = length(lightv);
			//正規化し、単位ベクトルにする
			lightv = normalize(lightv);
			//距離減衰係数
			float atten = 1.0f / (pointLights[i].lightatten.x + pointLights[i].lightatten.y * d +
				pointLights[i].lightatten.z * d * d);
			// ライトに向かうベクトルと法線の内積
			float3 dotlightnormal = dot(lightv, input.normal);
			// 反射光ベクトル
			float3 reflect = normalize(-lightv + 2 * dotlightnormal * input.normal);
			float3 specular = pow(saturate(dot(reflect, eyedir)), shininess);
			//トゥーン
			if (isToon) {
				specular = smoothstep(0.5f, 0.55f, specular);
				dotlightnormal = (1.0f - specular) * smoothstep(0.5f, 0.55f, dotlightnormal);
				ambient = (1.0f - dotlightnormal - specular) * ambient;
			}

			// 拡散反射光
			float3 diffuse = dotlightnormal * m_diffuse * diffuseColor;
			// 鏡面反射光
			specular = specular * m_specular * specularColor;

			// 全て加算する
			shadecolor.rgb += atten * (diffuse + specular) * pointLights[i].lightcolor;
		}
	}

	//スポットライト
	for (int i = 0; i < SPOTLIGHT_NUM; i++)
	{
		if (spotLights[i].active)
		{
			//ライトへの方向ベクトル
			float3 lightv = spotLights[i].lightpos - input.worldpos.xyz;
			float d = length(lightv);
			lightv = normalize(lightv);
			//距離減衰係数
			float atten = saturate(1.0f / (spotLights[i].lightatten.x + spotLights[i].lightatten.y
				* d + spotLights[i].lightatten.z * d * d));
			//角度減衰
			float cos = dot(lightv, spotLights[i].lightv);
			//減衰開始角度から、減衰終了角度にかけて減衰
			//減衰開始角度の内側は、1倍 減衰終了角度の外側は0倍の輝度
			float angleatten = smoothstep(spotLights[i].lightfactoranglecos.y,
				spotLights[i].lightfactoranglecos.x, cos);
			//角度減衰を乗算
			atten *= angleatten;
			//ライトに向かうベクトルと法線の内積
			float3 dotlightnormal = dot(lightv, input.normal);
			//反射光ベクトル　
			float3 reflect = normalize(-lightv + 2 * dotlightnormal * input.normal);
			float3 specular = pow(saturate(dot(reflect, eyedir)), shininess);
			//トゥーン
			if (isToon) {
				specular = smoothstep(0.5f, 0.55f, specular);
				dotlightnormal = (1.0f - specular) * smoothstep(0.5f, 0.55f, dotlightnormal);
				ambient = (1.0f - dotlightnormal - specular) * ambient;
			}

			// 拡散反射光
			float3 diffuse = dotlightnormal * m_diffuse * diffuseColor;
			// 鏡面反射光
			specular = specular * m_specular * specularColor;
			//全て加算する
			shadecolor.rgb += atten * (diffuse + specular) * spotLights[i].lightcolor;
		}
	}

	//丸影
	for (int i = 0; i < CIRCLESHADOW_NUM; i++)
	{
		if (circleShadows[i].active)
		{
			//ライトへの方向ベクトル
			float3 casterv = circleShadows[i].casterPos - input.worldpos.xyz;
			//投影方向での距離
			float d = dot(casterv, circleShadows[i].dir);
			//距離減衰係数
			float atten = saturate(1.0f / (circleShadows[i].atten.x + circleShadows[i].atten.y
				* d + circleShadows[i].atten.z * d * d));
			//距離がマイナスなら0にする
			atten *= step(0, d);
			//仮想ライトの座標
			float3 lightpos = circleShadows[i].casterPos + circleShadows[i].dir
				* circleShadows[i].distanceCasterLight;
			//オブジェクト表面からライトへのベクトル（単位ベクトル）
			float3 lightv = normalize(lightpos - input.worldpos.xyz);
			//角度減衰
			float cos = dot(lightv, circleShadows[i].dir);
			//減衰開始角度から、減衰終了角度にかけて減衰
			//減衰開始角度の内側は、1倍 減衰終了角度の外側は0倍の輝度
			float angleatten = smoothstep(circleShadows[i].factorAngleCos.y,
				circleShadows[i].factorAngleCos.x, cos);
			//角度減衰を乗算
			atten *= angleatten;

			//全て減算する
			shadecolor.rgb -= atten;
		}
	}

	//環境光
	shadecolor.rgb += ambient;

	//リムライト
	//内積
	float dotL = 1.0f;
	if (isRimLight)
	{
		dotL = smoothstep(0.5f, 0.55f, dot(eyedir, input.normal));
	}

	// シェーディングによる色で描画
	float4 DSC = dotL * shadecolor;
	float4 RIM = float4(rimColor.rgb, 1.0f) * (1.0f - dotL);
	float4 RGBA = (DSC * texcolor * color) + RIM;
	float4 RGBA2 = (DSC * color) + RIM;
	float3 RGB = RGBA.rgb;
	float  A = RGBA.a;

	if (isFog == true)
	{
		//フォグ
		float4 m_FogColor = float4(1.0f, 1.0f, 1.0f, 1.0f);                  //フォグカラー
		float  m_Near = 100.0f;             //フォグの開始位置
		float  m_Far = 500.0f;             //フォグの終了位置
		float  m_FogLen = m_Far - m_Near;             //m_Far - m_Nearの結果

		//頂点と視点との距離を計算する
		float d = distance(input.worldpos.xyz, cameraPos);

		float f = (m_Far - d) / (m_Far - m_Near);
		f = clamp(f, 0.0f, 1.0f);
		//オブジェクトのランバート拡散照明の計算結果とフォグカラーを線形合成する

		RGBA = RGBA * f + m_FogColor * (1.0f - f);
	}

	//一枚目の一つ目
	output.col = RGBA;
	//〃二つ目
	output.col2 = float4(1 - (RGBA).rgb, 1);
	//三つ目
	float y = dot(float3(0.299f, 0.587f, 0.114f), output.col);
	output.highLumi = y > 0.99f ? output.col : 0.0f;

	return output;
}