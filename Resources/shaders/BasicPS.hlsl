#include "Basic.hlsli"

Texture2D<float4> tex : register(t0); //0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);      //0番スロットに設定されたサンプラー


float4 main(VSOutput input) : SV_TARGET
{
	// テクスチャマッピング
	float4 texcolor = tex.Sample(smp, input.uv);

	// 光沢度
	const float shininess = 4.0f;
	// 頂点から視点への方向ベクトル
	float3 eyedir = normalize(cameraPos - input.worldpos.xyz);

	// 環境反射光
	float3 ambient = 0.06f;

	// シェーディングによる色
	float4 shadecolor = float4(ambientColor * ambient, 1.0f);

	//平行光源
	for (int i = 0; i < DIRLIGHT_NUM; i++) {
		if (dirLights[i].active) {
			// ライトに向かうベクトルと法線の内積
			float3 dotlightnormal = dot(dirLights[i].lightv, input.normal);
			// 反射光ベクトル
			float3 reflect = normalize(-dirLights[i].lightv + 2 * dotlightnormal * input.normal);
			// 拡散反射光
			float3 diffuse = dotlightnormal * 0.8f;
			// 鏡面反射光
			float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * 0.1f;

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
			// 拡散反射光
			float3 diffuse = dotlightnormal * 0.8f;
			// 鏡面反射光
			float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * 0.1f;

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
			// ライトに向かうベクトルと法線の内積
			float3 dotlightnormal = dot(lightv, input.normal);
			// 反射光ベクトル
			float3 reflect = normalize(-lightv + 2 * dotlightnormal * input.normal);
			// 拡散反射光
			float3 diffuse = dotlightnormal * 0.8f;
			// 鏡面反射光
			float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * 0.1f;
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

	// シェーディングによる色で描画
	float4 RGBA = (shadecolor * texcolor * color);
	float4 RGBA2 = (shadecolor * color);
	float3 RGB = RGBA.rgb;
	float  A = RGBA.a;

	float w, h, levels;
	//ミップマップ、横幅、縦幅、ミップマップレベル
	tex.GetDimensions(0, w, h, levels);

	float dx = 1.0f / w;
	float dy = 1.0f / h;
	float4 ret = float4(0, 0, 0, 0);
	bool isEffect = false;

	//ぼかし
	if (isGaussian == true)
	{
		ret += tex.Sample(smp, input.uv + float2(-2 * dx, -2 * dy)); // 左上 
		ret += tex.Sample(smp, input.uv + float2(0, -2 * dy)); // 上 
		ret += tex.Sample(smp, input.uv + float2(2 * dx, -2 * dy));// 右 上 
		ret += tex.Sample(smp, input.uv + float2(-2 * dx, 0)); // 左 
		ret += tex.Sample(smp, input.uv);                       // 自分 
		ret += tex.Sample(smp, input.uv + float2(2 * dx, 0)); // 右
		ret += tex.Sample(smp, input.uv + float2(-2 * dx, 2 * dy)); // 左下 
		ret += tex.Sample(smp, input.uv + float2(0, 2 * dy));// 下 
		ret += tex.Sample(smp, input.uv + float2(2 * dx, 2 * dy)); // 右 下
		ret /= 9.0f;

		isEffect = true;
	}

	//エンボス
	if (isEmboss == true)
	{
		ret += tex.Sample(smp, input.uv + float2(-2 * dx, -2 * dy)) * 2; // 左上
		ret += tex.Sample(smp, input.uv + float2(0, -2 * dy)); // 上
		ret += tex.Sample(smp, input.uv + float2(2 * dx, -2 * dy)) * 0; // 右 上
		ret += tex.Sample(smp, input.uv + float2(-2 * dx, 0)); // 左
		ret += tex.Sample(smp, input.uv); // 自分
		ret += tex.Sample(smp, input.uv + float2(2 * dx, 0)) * -1; // 右
		ret += tex.Sample(smp, input.uv + float2(-2 * dx, 2 * dy)) * 0; // 左下
		ret += tex.Sample(smp, input.uv + float2(0, 2 * dy)) * -1;// 下 
		ret += tex.Sample(smp, input.uv + float2(2 * dx, 2 * dy)) * -2; // 右 下 

		isEffect = true;
	}

	//シャープネス
	if (isSharpness == true)
	{
		ret += tex.Sample(smp, input.uv + float2(-2 * dx, -2 * dy)) * 0; // 左上 
		ret += tex.Sample(smp, input.uv + float2(0, -2 * dy)); // 上 
		ret += tex.Sample(smp, input.uv + float2(2 * dx, -2 * dy)) * 0;// 右 上 
		ret += tex.Sample(smp, input.uv + float2(-2 * dx, 0)); // 左 
		ret += tex.Sample(smp, input.uv) * 5.0f;                       // 自分 
		ret += tex.Sample(smp, input.uv + float2(2 * dx, 0)); // 右
		ret += tex.Sample(smp, input.uv + float2(-2 * dx, 2 * dy)) * 0; // 左下 
		ret += tex.Sample(smp, input.uv + float2(0, 2 * dy));// 下 
		ret += tex.Sample(smp, input.uv + float2(2 * dx, 2 * dy)) * 0; // 右 下

		isEffect = true;
	}

	//輪郭
	if (isOutLine == true)
	{
		ret += tex.Sample(smp, input.uv + float2(0, -2 * dy)) * -1; // 上
		ret += tex.Sample(smp, input.uv + float2(-2 * dx, 0)) * -1; // 左
		ret += tex.Sample(smp, input.uv) * 4; // 自分 
		ret += tex.Sample(smp, input.uv + float2(2 * dx, 0)) * -1; // 右 
		ret += tex.Sample(smp, input.uv + float2(0, 2 * dy)) * -1; // 下 
		// 反転 
		float Y = dot(ret.rgb * RGBA.rgb, float3(0.299, 0.587, 0.114));
		Y = pow(1.0f - Y, 10.0f);
		Y = step(0.2, Y);
		ret *= float4(Y, Y, Y, A);

		isEffect = true;
	}

	//ガウシアン
	if (isGaussian2 == true)
	{
		float dx = 3.0f / w;
		float dy = 3.0f / h;
		// 今 の ピクセル を 中心 に 縦横 5 つ ずつ に なる よう 加算 する 
		// 最 上段 
		ret += tex.Sample(smp, input.uv + float2(-2 * dx, 2 * dy)) * 1 / 256;
		ret += tex.Sample(smp, input.uv + float2(-1 * dx, 2 * dy)) * 4 / 256;
		ret += tex.Sample(smp, input.uv + float2(0 * dx, 2 * dy)) * 6 / 256;
		ret += tex.Sample(smp, input.uv + float2(1 * dx, 2 * dy)) * 4 / 256;
		ret += tex.Sample(smp, input.uv + float2(2 * dx, 2 * dy)) * 1 / 256;
		// 1 つ 上段 
		ret += tex.Sample(smp, input.uv + float2(-2 * dx, 1 * dy)) * 4 / 256;
		ret += tex.Sample(smp, input.uv + float2(-1 * dx, 1 * dy)) * 16 / 256;
		ret += tex.Sample(smp, input.uv + float2(0 * dx, 1 * dy)) * 24 / 256;
		ret += tex.Sample(smp, input.uv + float2(1 * dx, 1 * dy)) * 16 / 256;
		ret += tex.Sample(smp, input.uv + float2(2 * dx, 1 * dy)) * 4 / 256;
		// 中段 
		ret += tex.Sample(smp, input.uv + float2(-2 * dx, 0 * dy)) * 6 / 256;
		ret += tex.Sample(smp, input.uv + float2(-1 * dx, 0 * dy)) * 24 / 256;
		ret += tex.Sample(smp, input.uv + float2(0 * dx, 0 * dy)) * 36 / 256;
		ret += tex.Sample(smp, input.uv + float2(1 * dx, 0 * dy)) * 24 / 256;
		ret += tex.Sample(smp, input.uv + float2(2 * dx, 0 * dy)) * 6 / 256;
		// 1 つ 下段 
		ret += tex.Sample(smp, input.uv + float2(-2 * dx, -1 * dy)) * 4 / 256;
		ret += tex.Sample(smp, input.uv + float2(-1 * dx, -1 * dy)) * 16 / 256;
		ret += tex.Sample(smp, input.uv + float2(0 * dx, -1 * dy)) * 24 / 256;
		ret += tex.Sample(smp, input.uv + float2(1 * dx, -1 * dy)) * 16 / 256;
		ret += tex.Sample(smp, input.uv + float2(2 * dx, -1 * dy)) * 4 / 256;
		// 最 下段 
		ret += tex.Sample(smp, input.uv + float2(-2 * dx, -2 * dy)) * 1 / 256;
		ret += tex.Sample(smp, input.uv + float2(-1 * dx, -2 * dy)) * 4 / 256;
		ret += tex.Sample(smp, input.uv + float2(0 * dx, -2 * dy)) * 6 / 256;
		ret += tex.Sample(smp, input.uv + float2(1 * dx, -2 * dy)) * 4 / 256;
		ret += tex.Sample(smp, input.uv + float2(2 * dx, -2 * dy)) * 1 / 256;

		isEffect = true;


	}

	//ガウシアン２
	{
		/*ret += bkweights[0] * RGBA;
		for (int i = 1; i < 8; ++i)
		{
			ret += bkweights[i >> 2][i % 4] * tex.Sample(smp, input.uv + float2(i * dx, 0));
			ret += bkweights[i >> 2][i % 4] * tex.Sample(smp, input.uv + float2(-i * dx, 0));
		}
		return float4(ret.rgb * RGBA.rgb, A);*/
	}

	if (isGradation == true)
	{
		return float4(RGB - fmod(RGB, 0.25f), A);

		isEffect = true;
	}

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

		if (isEffect)
		{
			return RGBA2 * f + m_FogColor * (1.0f - f) * ret;
		}
		return RGBA * f + m_FogColor * (1.0f - f);
	}

	if (isEffect == true)
	{
		return ret * RGBA2;
	}

	return RGBA;
}
