#include"PeraShaderHeader.hlsli"

SamplerState smp : register(s0);

//高輝度と通常
struct PSOutput
{
    float4 color : SV_TARGET0; //通常
    float4 color2 : SV_TARGET1; //色違い
    float4 highLumi : SV_TARGET2; //高輝度
};

PSOutput PS(Output input) : SV_TARGET
{
    PSOutput output;

// シェーディングによる色で描画
    float4 RGBA = tex0.Sample(smp, input.uv);
    float3 RGB = RGBA.rgb;
    float A = RGBA.a;

    float w, h, levels;
//ミップマップ、横幅、縦幅、ミップマップレベル
    tex0.GetDimensions(0, w, h, levels);

    float dx = 1.0f / w;
    float dy = 1.0f / h;
    float4 ret = float4(0, 0, 0, 0);
    bool isEffect = false;

//シャープネス
    if (isSharpness == true)
    {
        ret += tex0.Sample(smp, input.uv + float2(-2 * dx, -2 * dy)) * 0; // 左上 
        ret += tex0.Sample(smp, input.uv + float2(0, -2 * dy)); // 上 
        ret += tex0.Sample(smp, input.uv + float2(2 * dx, -2 * dy)) * 0; // 右 上 
        ret += tex0.Sample(smp, input.uv + float2(-2 * dx, 0)); // 左 
        ret += tex0.Sample(smp, input.uv) * 5.0f; // 自分 
        ret += tex0.Sample(smp, input.uv + float2(2 * dx, 0)); // 右
        ret += tex0.Sample(smp, input.uv + float2(-2 * dx, 2 * dy)) * 0; // 左下 
        ret += tex0.Sample(smp, input.uv + float2(0, 2 * dy)); // 下 
        ret += tex0.Sample(smp, input.uv + float2(2 * dx, 2 * dy)) * 0; // 右 下

        isEffect = true;
    }

//輪郭
    if (isOutLine == true)
    {
        ret += tex0.Sample(smp, input.uv + float2(0, -2 * dy)) * -1; // 上
        ret += tex0.Sample(smp, input.uv + float2(-2 * dx, 0)) * -1; // 左
        ret += tex0.Sample(smp, input.uv) * 4; // 自分 
        ret += tex0.Sample(smp, input.uv + float2(2 * dx, 0)) * -1; // 右 
        ret += tex0.Sample(smp, input.uv + float2(0, 2 * dy)) * -1; // 下 
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
        ret += Gaussian2(tex0, smp, input.uv);

        isEffect = true;
    }

//ガウシアン２
    if (isGaussian2 == true)
    {
        ret += bkweights[0] * RGBA;
        for (int i = 1; i < 8; ++i)
        {
            ret += bkweights[i >> 2][i % 4] * tex0.Sample(smp, clamp(input.uv + float2(i * dx, 0), 0, 1.0f));
            ret += bkweights[i >> 2][i % 4] * tex0.Sample(smp, clamp(input.uv + float2(-i * dx, 0), 0, 1.0f));
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
        float4 Tex = tex0.Sample(smp, input.uv);
        float vignette = length(float2(0.5f, 0.5f) - input.uv);
        vignette = clamp(vignette - 0.45f, 0, 1);
        Tex.rgb -= vignette;
        ret = Tex;

        isEffect = true;
    }

//走査線
    if (isScanningLine)
    {
        float extend = 0.1f;
        float2 samplePoint = input.uv;
        float4 Tex = tex0.Sample(smp, samplePoint);
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
        tcolor.x = lerp(tcolor.r, y, grayScalePow);
        tcolor.y = lerp(tcolor.g, y, grayScalePow);
        tcolor.z = lerp(tcolor.b, y, grayScalePow);

		// 出力するピクセル色
        ret = float4(tcolor.rgb, 1.0f);

        isEffect = true;
    }

    //エンボス
    if (isEmboss == true)
    {
        ret += tex0.Sample(smp, input.uv + float2(-2 * dx, -2 * dy)) * 2; // 左上
        ret += tex0.Sample(smp, input.uv + float2(0, -2 * dy)); // 上
        ret += tex0.Sample(smp, input.uv + float2(2 * dx, -2 * dy)) * 0; // 右 上
        ret += tex0.Sample(smp, input.uv + float2(-2 * dx, 0)); // 左
        ret += tex0.Sample(smp, input.uv); // 自分
        ret += tex0.Sample(smp, input.uv + float2(2 * dx, 0)) * -1; // 右
        ret += tex0.Sample(smp, input.uv + float2(-2 * dx, 2 * dy)) * 0; // 左下
        ret += tex0.Sample(smp, input.uv + float2(0, 2 * dy)) * -1; // 下 
        ret += tex0.Sample(smp, input.uv + float2(2 * dx, 2 * dy)) * -2; // 右 下 

        isEffect = true;
    }
    
    //樽状
    if (isBarrelCurve)
    {
        float2 samplePoint = input.uv;
        samplePoint -= float2(0.5, 0.5);
        float distPower = pow(length(samplePoint), 0.1 * barrelCurvePow);
        samplePoint *= float2(distPower, distPower);
        samplePoint += float2(0.5, 0.5);
        float4 Tex = tex0.Sample(smp, samplePoint);
        ret = Tex;

        isEffect = true;
    }
    
//モザイク
    if (isMosaic)
    {
        float density = 50.0f;
        ret = tex0.Sample(smp, floor(input.uv * density) / density);

        isEffect = true;
    }

//ネガポジ
    if (isNega)
    {
        ret = float4(1.0f - RGBA.r, 1.0f - RGBA.g, 1.0f - RGBA.b, 1.0f);

        isEffect = true;
    }

//RGBずらし
    if (isRGBShift)
    {
        float shift = RGBShiftPow;
        float r = tex0.Sample(smp, input.uv + float2(-shift, 0)).r;
        float g = tex0.Sample(smp, input.uv + float2(0, 0)).g;
        float b = tex0.Sample(smp, input.uv + float2(shift, 0)).b;

        ret = float4(r, g, b, 1.0f);

        isEffect = true;
    }

    if (isMultiTex)
    {
        float dx = 1.0f / w;
        float dy = 1.0f / h;
	// 今 の ピクセル を 中心 に 縦横 5 つ ずつ に なる よう 加算 する 
	// 最 上段 
        ret += tex0.Sample(smp, clamp(input.uv + float2(-2 * dx, 2 * dy), 0, 1.0f)) * 1 / 256;
        ret += tex0.Sample(smp, clamp(input.uv + float2(-1 * dx, 2 * dy), 0, 1.0f)) * 4 / 256;
        ret += tex0.Sample(smp, clamp(input.uv + float2(0 * dx, 2 * dy), 0, 1.0f)) * 6 / 256;
        ret += tex0.Sample(smp, clamp(input.uv + float2(1 * dx, 2 * dy), 0, 1.0f)) * 4 / 256;
        ret += tex0.Sample(smp, clamp(input.uv + float2(2 * dx, 2 * dy), 0, 1.0f)) * 1 / 256;
	// 1 つ 上段 
        ret += tex0.Sample(smp, clamp(input.uv + float2(-2 * dx, 1 * dy), 0, 1.0f)) * 4 / 256;
        ret += tex0.Sample(smp, clamp(input.uv + float2(-1 * dx, 1 * dy), 0, 1.0f)) * 16 / 256;
        ret += tex0.Sample(smp, clamp(input.uv + float2(0 * dx, 1 * dy), 0, 1.0f)) * 24 / 256;
        ret += tex0.Sample(smp, clamp(input.uv + float2(1 * dx, 1 * dy), 0, 1.0f)) * 16 / 256;
        ret += tex0.Sample(smp, clamp(input.uv + float2(2 * dx, 1 * dy), 0, 1.0f)) * 4 / 256;
	// 中段 
        ret += tex0.Sample(smp, clamp(input.uv + float2(-2 * dx, 0 * dy), 0, 1.0f)) * 6 / 256;
        ret += tex0.Sample(smp, clamp(input.uv + float2(-1 * dx, 0 * dy), 0, 1.0f)) * 24 / 256;
        ret += tex0.Sample(smp, clamp(input.uv + float2(0 * dx, 0 * dy), 0, 1.0f)) * 36 / 256;
        ret += tex0.Sample(smp, clamp(input.uv + float2(1 * dx, 0 * dy), 0, 1.0f)) * 24 / 256;
        ret += tex0.Sample(smp, clamp(input.uv + float2(2 * dx, 0 * dy), 0, 1.0f)) * 6 / 256;
	// 1 つ 下段 
        ret += tex0.Sample(smp, clamp(input.uv + float2(-2 * dx, -1 * dy), 0, 1.0f)) * 4 / 256;
        ret += tex0.Sample(smp, clamp(input.uv + float2(-1 * dx, -1 * dy), 0, 1.0f)) * 16 / 256;
        ret += tex0.Sample(smp, clamp(input.uv + float2(0 * dx, -1 * dy), 0, 1.0f)) * 24 / 256;
        ret += tex0.Sample(smp, clamp(input.uv + float2(1 * dx, -1 * dy), 0, 1.0f)) * 16 / 256;
        ret += tex0.Sample(smp, clamp(input.uv + float2(2 * dx, -1 * dy), 0, 1.0f)) * 4 / 256;
	// 最 下段 
        ret += tex0.Sample(smp, clamp(input.uv + float2(-2 * dx, -2 * dy), 0, 1.0f)) * 1 / 256;
        ret += tex0.Sample(smp, clamp(input.uv + float2(-1 * dx, -2 * dy), 0, 1.0f)) * 4 / 256;
        ret += tex0.Sample(smp, clamp(input.uv + float2(0 * dx, -2 * dy), 0, 1.0f)) * 6 / 256;
        ret += tex0.Sample(smp, clamp(input.uv + float2(1 * dx, -2 * dy), 0, 1.0f)) * 4 / 256;
        ret += tex0.Sample(smp, clamp(input.uv + float2(2 * dx, -2 * dy), 0, 1.0f)) * 1 / 256;

        isEffect = true;
    }

    if (isEffect)
    {
        output.color = ret * color;
    }
    else
    {
        output.color = RGBA * color;
    }

//ブルーム
    if (isBloom || isCrossFilter)
    {
        float4 col = tex0.Sample(smp, input.uv);
        float grayScale = col.r * 0.299 + col.g * 0.587 + col.b * 0.144;
        float extract = smoothstep(0.6, 0.9, grayScale);

        output.highLumi = col * extract;

	//ドットフィルタ
        if (isCrossFilter)
        {
            float st = input.uv / w * 20;
            st = frac(st * float2(w, h));
            float l = distance(st, float2(0.5, 0.5));
            output.highLumi = output.highLumi * float4(1, 1, 1, 1) * 1 - step(0.3, l);
        }
    }

//色違い
    output.color2 = float4(1.0f - RGBA.r, 1.0f - RGBA.g, 1.0f - RGBA.b, 1.0f);

    return output;
}