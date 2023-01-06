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

			//ぼかし
			//ret += tex.Sample(smp, input.uv + float2(-2 * dx, -2 * dy)); // 左上 
			//ret += tex.Sample(smp, input.uv + float2(0, -2 * dy)); // 上 
			//ret += tex.Sample(smp, input.uv + float2(2 * dx, -2 * dy));// 右 上 
			//ret += tex.Sample(smp, input.uv + float2(-2 * dx, 0)); // 左 
			//ret += tex.Sample(smp, input.uv);                       // 自分 
			//ret += tex.Sample(smp, input.uv + float2(2 * dx, 0)); // 右
			//ret += tex.Sample(smp, input.uv + float2(-2 * dx, 2 * dy)); // 左下 
			//ret += tex.Sample(smp, input.uv + float2(0, 2 * dy));// 下 
			//ret += tex.Sample(smp, input.uv + float2(2 * dx, 2 * dy)); // 右 下
			//return ret / 9.0f * RGBA;

			//シャープネス
			//ret += tex.Sample(smp, input.uv + float2(-2 * dx, -2 * dy)) * 0; // 左上 
			//ret += tex.Sample(smp, input.uv + float2(0, -2 * dy)); // 上 
			//ret += tex.Sample(smp, input.uv + float2(2 * dx, -2 * dy)) * 0;// 右 上 
			//ret += tex.Sample(smp, input.uv + float2(-2 * dx, 0)); // 左 
			//ret += tex.Sample(smp, input.uv) * 5.0f;                       // 自分 
			//ret += tex.Sample(smp, input.uv + float2(2 * dx, 0)); // 右
			//ret += tex.Sample(smp, input.uv + float2(-2 * dx, 2 * dy)) * 0; // 左下 
			//ret += tex.Sample(smp, input.uv + float2(0, 2 * dy));// 下 
			//ret += tex.Sample(smp, input.uv + float2(2 * dx, 2 * dy)) * 0; // 右 下
			//return ret * RGBA;

			//輪郭
			//ret += tex.Sample(smp, input.uv + float2(0, -2 * dy)) * -1; // 上
			//ret += tex.Sample(smp, input.uv + float2(-2 * dx, 0)) * -1; // 左
			//ret += tex.Sample(smp, input.uv) * 4; // 自分 
			//ret += tex.Sample(smp, input.uv + float2(2 * dx, 0)) * -1; // 右 
			//ret += tex.Sample(smp, input.uv + float2(0, 2 * dy)) * -1; // 下 
			//// 反転 
			//float Y = dot(ret.rgb * RGBA2.rgb, float3(0.299, 0.587, 0.114));
			//Y = pow(1.0f - Y, 10.0f);
			//Y = step(0.2, Y);
			//return float4(Y, Y, Y, A);

			//ガウシアン
			{
				float dx = 2.0f / w;
				//float dy = 2.0f / h;
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

				return ret * RGBA;
			}


			//諧調
			//return float4(RGB - fmod(RGB, 0.25f), A);
}
