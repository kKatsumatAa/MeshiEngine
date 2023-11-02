#include "CommonHeader.hlsli"


PSOutput main(VSOutput input)
{
    PSOutput output;

	// テクスチャマッピング
    float4 texcolor = tex.Sample(smp, input.uv);

	// 光沢度
    const float SHININESS = 4.0f;
	// 頂点から視点への方向ベクトル
    float3 eyedir = normalize(cameraPos - input.worldpos.xyz);

	// 環境反射光
    float3 ambient = ambientColor * 0.06f;

	// シェーディングによる色
    float4 shadecolor = { 0, 0, 0, 1.0f };
	
	    //スペキュラマップの色
    float4 specularMapCol = { 0, 0, 0, 0 };

	    //ワールドの法線
    float3 wNormal = normalize(mul(world, float4(input.normal, 0))).rgb;
    
    float3 lightNormal = wNormal;
    
    
    //シャドウマップ--------------------------
    //テクスチャ座標への変換(tpos.zが深度値)
    float3 posFromLightVP = input.tpos.xyz / input.tpos.w;
    float2 shadowUV = (input.tpos.xy / input.tpos.w + float2(1, -1)) * float2(0.5, -0.5);
    //影テクスチャの値
    float depthFromLight = lightDepthTex.SampleCmpLevelZero(
    shadowSmp, //比較サンプラー
    shadowUV, //uv
    posFromLightVP.z - 0.0005f //比較対象値
    ).x;
    //ライトから見て最初の撮影結果より遠いと影になる(0.5~1.0fの線形補間)
    texcolor.xyz *= (lerp(0.5f, 1.0f, depthFromLight));
  
    
    //ノーマルマップが有効ならライトの計算に使う法線を算出
    if (isNormalMap)
    {
        //ローカルの法線を使わないといけない
        lightNormal = GetNormalMapWorldNormalVec(tex4.Sample(smp, input.uv), input.tangent,
            input.binormal, input.normal, world);
    }
	
	
	//平行光源
    for (int i = 0; i < S_DIRLIGHT_NUM; i++)
    {
        if (dirLights[i].active)
        {
			// ライトに向かうベクトルと法線の内積
            float3 dotlightnormal = dot(dirLights[i].lightv, lightNormal);
			// 反射光ベクトル
            float3 reflect = normalize(-dirLights[i].lightv + 2 * dotlightnormal * lightNormal);
            float3 specular = pow(saturate(dot(reflect, eyedir)), SHININESS);
			//トゥーン
            if (isToon)
            {
                specular = smoothstep(0.5f, 0.55f, specular);
                dotlightnormal = (1.0f - specular) * smoothstep(0.5f, 0.55f, dotlightnormal);
                ambient = (1.0f - dotlightnormal - specular) * ambient;
            }

			// 拡散反射光
            float3 diffuse = dotlightnormal * 0.8f * diffuseColor;
			// 鏡面反射光
            specular = specular * 0.9f * specularColor;

			// 全て加算する
            shadecolor.rgb += (diffuse + specular) * dirLights[i].lightcolor;
			
			//スペキュラマップ
            if (isSpecularMap)
            {
                float4 specularMap = GetSpecularMapColor(specular, diffuse, tex2.Sample(smp, input.uv));
                specularMapCol += specularMap;
            }
        }
    }

	//点光源
    for (int j = 0; j < S_POINTLIGHT_NUM; j++)
    {
        if (pointLights[j].active)
        {
			//ライトへのベクトル
            float3 lightv = pointLights[j].lightpos - input.worldpos.xyz;
			//ベクトルの長さ
            float d = length(lightv);
			//正規化し、単位ベクトルにする
            lightv = normalize(lightv);
			//距離減衰係数
            float atten = 1.0f / (pointLights[j].lightatten.x + pointLights[j].lightatten.y * d +
				pointLights[j].lightatten.z * d * d);
			// ライトに向かうベクトルと法線の内積
            float3 dotlightnormal = dot(lightv, lightNormal);
			// 反射光ベクトル
            float3 reflect = normalize(-lightv + 2 * dotlightnormal * lightNormal);
            float3 specular = pow(saturate(dot(reflect, eyedir)), SHININESS);
			//トゥーン
            if (isToon)
            {
                specular = smoothstep(0.5f, 0.55f, specular);
                dotlightnormal = (1.0f - specular) * smoothstep(0.5f, 0.55f, dotlightnormal);
                ambient = (1.0f - dotlightnormal - specular) * ambient;
            }

			// 拡散反射光
            float3 diffuse = dotlightnormal * 0.8f * diffuseColor;
			// 鏡面反射光
            specular = specular * 0.9f * specularColor;

			// 全て加算する
            shadecolor.rgb += atten * (diffuse + specular) * pointLights[j].lightcolor;
			
			//スペキュラマップ
            if (isSpecularMap)
            {
                float4 specularMap = GetSpecularMapColor(specular, diffuse, tex2.Sample(smp, input.uv));
                specularMapCol += specularMap;
            }
        }
    }

	//スポットライト
    for (int k = 0; k < S_SPOTLIGHT_NUM; k++)
    {
        if (spotLights[k].active)
        {
			//ライトへの方向ベクトル
            float3 lightv = spotLights[k].lightpos - input.worldpos.xyz;
            float d = length(lightv);
            lightv = normalize(lightv);
			//距離減衰係数
            float atten = saturate(1.0f / (spotLights[k].lightatten.x + spotLights[k].lightatten.y
				* d + spotLights[k].lightatten.z * d * d));
			//角度減衰
            float cos = dot(lightv, spotLights[k].lightv);
			//減衰開始角度から、減衰終了角度にかけて減衰
			//減衰開始角度の内側は、1倍 減衰終了角度の外側は0倍の輝度
            float angleatten = smoothstep(spotLights[k].lightfactoranglecos.y,
				spotLights[k].lightfactoranglecos.x, cos);
			//角度減衰を乗算
            atten *= angleatten;
			// ライトに向かうベクトルと法線の内積
            float3 dotlightnormal = dot(lightv, lightNormal);
			// 反射光ベクトル
            float3 reflect = normalize(-lightv + 2 * dotlightnormal * lightNormal);
            float3 specular = pow(saturate(dot(reflect, eyedir)), SHININESS);
			//トゥーン
            if (isToon)
            {
                specular = smoothstep(0.5f, 0.55f, specular);
                dotlightnormal = (1.0f - specular) * smoothstep(0.5f, 0.55f, dotlightnormal);
                ambient = (1.0f - dotlightnormal - specular) * ambient;
            }

			// 拡散反射光
            float3 diffuse = dotlightnormal * 0.8f * diffuseColor;
			// 鏡面反射光
            specular = specular * 0.9f * specularColor;

			//全て加算する
            shadecolor.rgb += atten * (diffuse + specular) * spotLights[k].lightcolor;
			
			//スペキュラマップ
            if (isSpecularMap)
            {
                float4 specularMap = GetSpecularMapColor(specular, diffuse, tex2.Sample(smp, input.uv));
                specularMapCol += specularMap;
            }
        }
    }

	//丸影
    for (int l = 0; l < S_CIRCLESHADOW_NUM; l++)
    {
        if (circleShadows[l].active)
        {
			//ライトへの方向ベクトル
            float3 casterv = circleShadows[l].casterPos - input.worldpos.xyz;
			//投影方向での距離
            float d = dot(casterv, circleShadows[l].dir);
			//距離減衰係数
            float atten = saturate(1.0f / (circleShadows[l].atten.x + circleShadows[l].atten.y
				* d + circleShadows[l].atten.z * d * d));
			//距離がマイナスなら0にする
            atten *= step(0, d);
			//仮想ライトの座標
            float3 lightpos = circleShadows[l].casterPos + circleShadows[l].dir
				* circleShadows[l].distanceCasterLight;
			//オブジェクト表面からライトへのベクトル（単位ベクトル）
            float3 lightv = normalize(lightpos - input.worldpos.xyz);
			//角度減衰
            float cos = dot(lightv, circleShadows[l].dir);
			//減衰開始角度から、減衰終了角度にかけて減衰
			//減衰開始角度の内側は、1倍 減衰終了角度の外側は0倍の輝度
            float angleatten = smoothstep(circleShadows[l].factorAngleCos.y,
				circleShadows[l].factorAngleCos.x, cos);
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
        dotL = smoothstep(0.5f, 0.55f, dot(eyedir, wNormal));
    }

	// シェーディングによる色で描画
    float4 DSC = dotL * shadecolor;
    float4 RIM = float4(rimColor.rgb, 1.0f) * (1.0f - dotL);
    float4 RGBA = (DSC * texcolor * color) + RIM;
    float4 RGBA2 = (DSC * color) + RIM;
    float3 RGB = RGBA.rgb;
    float A = RGBA.a;
	
	
	    //スペキュラマップ
    if (isSpecularMap)
    {
        RGBA += saturate(specularMapCol);
    }
	
		//疑似シルエット
    if (isSilhouette)
    {
        RGBA = float4(1, 0, 0, 1);
    }

    if (isFog)
    {
		//フォグ
        float4 m_FogColor = float4(1.0f, 1.0f, 1.0f, 1.0f); //フォグカラー
        float m_Near = 100.0f; //フォグの開始位置
        float m_Far = 500.0f; //フォグの終了位置
        float m_FogLen = m_Far - m_Near; //m_Far - m_Nearの結果

		//頂点と視点との距離を計算する
        float d = distance(input.worldpos.xyz, cameraPos);

        float f = (m_Far - d) / (m_Far - m_Near);
        f = clamp(f, 0.0f, 1.0f);
		//オブジェクトのランバート拡散照明の計算結果とフォグカラーを線形合成する

        RGBA = RGBA * f + m_FogColor * (1.0f - f);
    }
	
		//ディゾルブ
    if (isDissolve)
    {
        float4 mask = tex2.Sample(smp, input.uv);
		//R値が0.5超えると描画破棄する
        clip(mask.r - dissolveT);
    }

	//一枚目の一つ目
    output.col = RGBA;

    return output;
}
