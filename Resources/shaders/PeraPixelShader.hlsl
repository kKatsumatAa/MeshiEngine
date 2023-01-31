#include"PeraShaderHeader.hlsli"


float4 PS(Output input) : SV_TARGET
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
		ret = float4(Y, Y, Y, A);

		isEffect = true;
	}

	//ガウシアン
	if (isGaussian2 == true)
	{
		float dx = 2.0f / w;
		float dy = 2.0f / h;
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

	//諧調
	if (isGradation)
	{
		ret = float4(RGB - fmod(RGB, 0.25f), A);

		isEffect = true;
	}

	if (isEffect)
	{
		return ret;
	}

	return RGBA;
}