#include "ParticleLight.hlsli"

Texture2D<float4> tex : register(t0); // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); // 0番スロットに設定されたサンプラー

struct PSOutput
{
    float4 col : SV_TARGET0; //通常のレンダリング
    float4 col2 : SV_TARGET1; //法線
    float4 highLumi : SV_TARGET2; //高輝度
};


PSOutput main(GSOutput input)
{
    PSOutput output;

	// テクスチャマッピング
    float4 texcolor = tex.Sample(smp, input.uv);

	// 光沢度
    const float SHININESS = 20.0f;
	// 頂点から視点への方向ベクトル
    float3 eyedir = normalize(cameraPos - input.worldpos.xyz);

	// 環境反射光
    float3 ambient = float3(ambientColor * m_ambient);

	// シェーディングによる色
    float4 shadecolor = { 0, 0, 0, m_alpha };
    
    //スペキュラマップの色
    float4 specularMapCol = { 0, 0, 0, 0 };
    
    //ワールドの法線
    float3 wNormal = input.normal;
    
    float3 lightNormal = wNormal;
        
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

			// 拡散反射光
            float3 diffuse = dotlightnormal * m_diffuse * diffuseColor;
			// 鏡面反射光
            specular = specular * m_specular * specularColor;

			// 全て加算する
            shadecolor.rgb += (diffuse + specular) * dirLights[i].lightcolor;
        }
    }

	//点光源
    for (int i = 0; i < S_POINTLIGHT_NUM; i++)
    {
        if (pointLights[i].active)
        {
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
            float3 dotlightnormal = dot(lightv, lightNormal);
            
			// 反射光ベクトル
            float3 reflect = normalize(-lightv + 2 * dotlightnormal * lightNormal);
                        
            float3 specular = pow(saturate(dot(reflect, eyedir)), SHININESS);

			// 拡散反射光
            float3 diffuse = dotlightnormal * m_diffuse * diffuseColor;
			// 鏡面反射光
            specular = specular * m_specular * specularColor;
            
			// 全て加算する
            shadecolor.rgb += atten * (diffuse + specular) * pointLights[i].lightcolor;
        }
    }
    

	//スポットライト
    for (int i = 0; i < S_SPOTLIGHT_NUM; i++)
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
            float3 dotlightnormal = dot(lightv, lightNormal);
			//反射光ベクトル　
            float3 reflect = normalize(-lightv + 2 * dotlightnormal * lightNormal);
            float3 specular = pow(saturate(dot(reflect, eyedir)), SHININESS);

			// 拡散反射光
            float3 diffuse = dotlightnormal * m_diffuse * diffuseColor;
			// 鏡面反射光
            specular = specular * m_specular * specularColor;
			//全て加算する
            shadecolor.rgb += atten * (diffuse + specular) * spotLights[i].lightcolor;
        }
    }

	//丸影
    for (int i = 0; i < S_CIRCLESHADOW_NUM; i++)
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

	// シェーディングによる色で描画
    float4 RGBA = (shadecolor * texcolor * input.color);
    
	//一枚目の一つ目
    output.col = RGBA;

    return output;
}