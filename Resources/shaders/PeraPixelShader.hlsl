#include"PeraShaderHeader.hlsli"

Texture2D<float4> tex : register(t0);
//ガラスフィルター
Texture2D<float4> effectTex : register(t1);

SamplerState smp : register(s0);

PixelOutput PS(Output input) : SV_TARGET
{
	// シェーディングによる色で描画
	float4 RGBA = tex.Sample(smp, input.uv);
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
	//if (isGaussian == true)
	//{
	//	ret += tex.Sample(smp, input.uv + float2(-2 * dx, -2 * dy)); // 左上 
	//	ret += tex.Sample(smp, input.uv + float2(0, -2 * dy)); // 上 
	//	ret += tex.Sample(smp, input.uv + float2(2 * dx, -2 * dy));// 右 上 
	//	ret += tex.Sample(smp, input.uv + float2(-2 * dx, 0)); // 左 
	//	ret += tex.Sample(smp, input.uv);                       // 自分 
	//	ret += tex.Sample(smp, input.uv + float2(2 * dx, 0)); // 右
	//	ret += tex.Sample(smp, input.uv + float2(-2 * dx, 2 * dy)); // 左下 
	//	ret += tex.Sample(smp, input.uv + float2(0, 2 * dy));// 下 
	//	ret += tex.Sample(smp, input.uv + float2(2 * dx, 2 * dy)); // 右 下
	//	ret /= 9.0f;

	//	isEffect = true;
	//}

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
		ret = float4(Y, Y, Y, A);

		isEffect = true;
	}

	//ガウシアン
	if (isGaussian == true)
	{
		float dx = 2.0f / w;
		float dy = 2.0f / h;
		// 今 の ピクセル を 中心 に 縦横 5 つ ずつ に なる よう 加算 する 
		// 最 上段 
		ret += tex.Sample(smp, clamp(input.uv + float2(-2 * dx, 2 * dy),0,1.0f)) * 1 / 256;
		ret += tex.Sample(smp, clamp(input.uv + float2(-1 * dx, 2 * dy),0,1.0f)) * 4 / 256;
		ret += tex.Sample(smp, clamp(input.uv + float2(0 * dx, 2 * dy),0,1.0f)) * 6 / 256;
		ret += tex.Sample(smp, clamp(input.uv + float2(1 * dx, 2 * dy),0,1.0f)) * 4 / 256;
		ret += tex.Sample(smp, clamp(input.uv + float2(2 * dx, 2 * dy),0,1.0f)) * 1 / 256;
		// 1 つ 上段 
		ret += tex.Sample(smp, clamp(input.uv + float2(-2 * dx, 1 * dy),0,1.0f)) * 4 / 256;
		ret += tex.Sample(smp, clamp(input.uv + float2(-1 * dx, 1 * dy),0,1.0f)) * 16 / 256;
		ret += tex.Sample(smp, clamp(input.uv + float2(0 * dx, 1 * dy),0,1.0f)) * 24 / 256;
		ret += tex.Sample(smp, clamp(input.uv + float2(1 * dx, 1 * dy),0,1.0f)) * 16 / 256;
		ret += tex.Sample(smp, clamp(input.uv + float2(2 * dx, 1 * dy),0,1.0f)) * 4 / 256;
		// 中段 
		ret += tex.Sample(smp, clamp(input.uv + float2(-2 * dx, 0 * dy),0,1.0f)) * 6 / 256;
		ret += tex.Sample(smp, clamp(input.uv + float2(-1 * dx, 0 * dy),0,1.0f)) * 24 / 256;
		ret += tex.Sample(smp, clamp(input.uv + float2(0 * dx, 0 * dy),0,1.0f)) * 36 / 256;
		ret += tex.Sample(smp, clamp(input.uv + float2(1 * dx, 0 * dy),0,1.0f)) * 24 / 256;
		ret += tex.Sample(smp, clamp(input.uv + float2(2 * dx, 0 * dy),0,1.0f)) * 6 / 256;
		// 1 つ 下段 
		ret += tex.Sample(smp, clamp(input.uv + float2(-2 * dx, -1 * dy),0,1.0f)) * 4 / 256;
		ret += tex.Sample(smp, clamp(input.uv + float2(-1 * dx, -1 * dy),0,1.0f)) * 16 / 256;
		ret += tex.Sample(smp, clamp(input.uv + float2(0 * dx, -1 * dy),0,1.0f)) * 24 / 256;
		ret += tex.Sample(smp, clamp(input.uv + float2(1 * dx, -1 * dy),0,1.0f)) * 16 / 256;
		ret += tex.Sample(smp, clamp(input.uv + float2(2 * dx, -1 * dy),0,1.0f)) * 4 / 256;
		// 最 下段 
		ret += tex.Sample(smp, clamp(input.uv + float2(-2 * dx, -2 * dy),0,1.0f)) * 1 / 256;
		ret += tex.Sample(smp, clamp(input.uv + float2(-1 * dx, -2 * dy),0,1.0f)) * 4 / 256;
		ret += tex.Sample(smp, clamp(input.uv + float2(0 * dx, -2 * dy),0,1.0f)) * 6 / 256;
		ret += tex.Sample(smp, clamp(input.uv + float2(1 * dx, -2 * dy),0,1.0f)) * 4 / 256;
		ret += tex.Sample(smp, clamp(input.uv + float2(2 * dx, -2 * dy),0,1.0f)) * 1 / 256;

		isEffect = true;
	}

	//ガウシアン２
	if (isGaussian2 == true)
	{
		ret += bkweights[0] * RGBA;
		for (int i = 1; i < 8; ++i)
		{
			ret += bkweights[i >> 2][i % 4] * tex.Sample(smp, clamp(input.uv + float2(i * dx, 0),0,1.0f));
			ret += bkweights[i >> 2][i % 4] * tex.Sample(smp, clamp(input.uv + float2(-i * dx, 0),0,1.0f));
		}

		isEffect = true;
	}

	//諧調
	if (isGradation)
	{
		ret = float4(RGB - fmod(RGB, 0.25f), A);

		isEffect = true;
	}

	//ビネット
	if (isVignette)
	{
		float2 samplePoint = input.uv;
		float4 Tex = tex.Sample(smp, input.uv);
		float vignette = length(float2(0.5f, 0.5f) - input.uv);
		vignette = clamp(vignette - 0.4f, 0, 1);
		Tex.rgb -= vignette;
		ret = Tex;

		isEffect = true;
	}

	//樽状
	if (isBarrelCurve)
	{
		float2 samplePoint = input.uv;
		samplePoint -= float2(0.5, 0.5);
		float distPower = pow(length(samplePoint), 0.02);
		samplePoint *= float2(distPower, distPower);
		samplePoint += float2(0.5, 0.5);
		float4 Tex = tex.Sample(smp, samplePoint);
		ret = Tex;

		isEffect = true;
	}

	//走査線
	if (isScanningLine)
	{
		float extend = 0.1f;
		float2 samplePoint = input.uv;
		float4 Tex = tex.Sample(smp, samplePoint);
		float sinv = sin(input.uv.y * 2 + time * extend * -0.1);
		float steped = step(0.99, sinv * sinv);
		Tex.rgb -= (1 - steped) * abs(sin(input.uv.y * 50.0 + time * extend * 1.0)) * 0.05;
		Tex.rgb -= (1 - steped) * abs(sin(input.uv.y * 100.0 - time * extend * 2.0)) * 0.08;
		Tex.rgb += steped * 0.1;
		ret = Tex;

		isEffect = true;
	}

	//グレー
	if (isGrayScale)
	{
		// テクスチャから画素値取得
		float4 tcolor = RGBA;

			float y, u, v;
			y = tcolor.x * 0.199 + tcolor.y * 0.487 + tcolor.z * 0.014;

			// グレースケール変換
			tcolor.x = y;
			tcolor.y = y;
			tcolor.z = y;

			// 出力するピクセル色
			ret = tcolor;

			isEffect = true;
	}

	//ブルーム用（複数出力）
	PixelOutput output;
	output.normal.rgb = float3((input.normal.xyz + 1.0f) / 2.0f);
	output.normal.a = 1;

	//ポストエフェクトない場合
	output.col = RGBA;

	//ある場合
	if (isEffect)
	{
		output.col = ret;
	}

	return output;
}