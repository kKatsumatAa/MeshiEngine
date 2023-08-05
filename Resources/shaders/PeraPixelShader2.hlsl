#include"PeraShaderHeader.hlsli"

SamplerState smp : register(s0);

float4 PS2(Output input) : SV_TARGET
{
	// シェーディングによる色で描画
    float4 RGBA = tex0.Sample(smp, input.uv);
    float3 RGB = RGBA.rgb;
    float A = RGBA.a;

    float w, h, level;

    tex0.GetDimensions(0, w, h, level);

    float dx = 1.0f / w;
    float dy = 1.0f / h;

    float4 ret = float4(0, 0, 0, 0);
    float4 col = tex0.Sample(smp, input.uv);

    bool isEffect = false;

    if (isGaussian2)
    {
        ret += bkweights[0] * col;
        for (int i = 1; i < 8; ++i)
        {
            ret += bkweights[i >> 2][i % 4] * tex0.Sample(smp, clamp(input.uv + float2(0, dy * i), 0, 1.0f));
            ret += bkweights[i >> 2][i % 4] * tex0.Sample(smp, clamp(input.uv + float2(0, -dy * i), 0, 1.0f));
        }

        isEffect = true;
    }
	
	//ラジアル
    if (isRadialBlur)
    {
        ret += GetRadialBlur(tex0, smp, float2(w, h) / 2.0f, input.uv, 0.0005f * radialPow, 0.021f * radialPow);

        isEffect = true;
    }

	//法線マップ
    if (isGlassFilter)
    {
        float2 nmTex = effectTex.Sample(smp, input.uv).xy;
        nmTex = nmTex * 2.0f - 1.0f;

		//nmTexの範囲は-1～1だが、幅1がテクスチャ1枚の
		//大きさであり-1～1では歪みすぎるため0.1を乗算している
        ret = tex0.Sample(smp, input.uv + nmTex * 0.1f);
        
        isEffect = true;
    }
    
     //ノイズ
    if (isNoise)
    {
        float c = noise(input.uv * 256.0f, time);
        return float4(c, c, c, 1.0f);
    }
   
        
    //深度値
    if (isDepthField)
    {
       // 深度値
        float depth = depthTex.Sample(smp, input.uv).r;
       // フォーカスあってる
        float inFocus = 1.0f - smoothstep(0, nFocusWidth, abs(depth - focusDepth));
        //フォーカスからの深度の差を測る
        float depthDiff = abs(inFocus - depth);
        //近い値でも差が出るようにpow
        depthDiff = pow(depthDiff, focusDiffPow);
        
        float2 uvSize = float2(1.0f, 0.5);
        float2 uvOfst = float2(0, 0);
        //最大1.0f のdepth値の差を複数の縮小バッファとブレンドできるように倍
        const int SHRINK_EXTEND = 6;
        float t = depthDiff * SHRINK_EXTEND;
        
        float no;
        //tの整数部分をnoへ、 小数部分をtへ分ける

        t = modf(t, no);
        
        float4 retColor[2];
        
        retColor[0] = tex0.Sample(smp, input.uv); //通常テクスチャ
        //整数部分が０

        if (no == 0.0f)
        {
            retColor[1] = Get5x5GaussianBlur(tex5, smp, input.uv * uvSize + uvOfst, dx, dy, float4(uvOfst, uvOfst + uvSize));
        }
        //整数部分があれば
        else
        {
            for (int i = 1; i <= SHRINK_EXTEND; ++i)
            {
                if (i - no < 0)
                {
                    continue;
                }
                retColor[i - no] = Get5x5GaussianBlur(tex5, smp, input.uv * uvSize + uvOfst, dx, dy, float4(uvOfst, uvOfst + uvSize));
                uvOfst.y += uvSize.y;
                uvSize *= 0.5f;
                if (i - no > 1.0f)
                {
                    break;
                }
            }
        }
        ret = lerp(retColor[0], retColor[1], t);
        
        isEffect = true;
    }


	//ブルーム
    if (isBloom)
    {
        float w, h, level;

        tex3.GetDimensions(0, w, h, level);

        float dx = 1.0f / w;
        float dy = 1.0f / h;

        float4 bloomAccum = float4(0, 0, 0, 0);
        float2 uvSize = float2(1.0, 0.5);
        float2 uvOfst = float2(0, 0);

		//iの上限を変えると強さが変わる（8にすると細かすぎておかしくなる）
        for (int i = 0; i < 6; ++i)
        {
            bloomAccum += Get5x5GaussianBlur(
				tex3, smp, input.uv * uvSize + uvOfst, dx, dy, float4(uvOfst, uvOfst + uvSize));
            uvOfst.y += uvSize.y;
            uvSize *= 0.5;
        }

		//元の画像とぼかした高輝度の部分を足す
        return tex0.Sample(smp, input.uv) //通常テクスチャ
			+ saturate(bloomAccum) / bloomPow; //縮小ぼかし済み(/ で強さ調整)
    }

	//クロスフィルタ
    if (isCrossFilter)
    {
        float w, h, level;

        tex3.GetDimensions(0, w, h, level);

        float dx = 1.0f / w;
        float dy = 1.0f / h;

        float4 bloomAccum[2] = { float4(0, 0, 0, 0), float4(0, 0, 0, 0) };
        float2 uvSize = float2(1.0, 0.5);
        float2 uvOfst = float2(0, 0);

		//iの上限を変えると強さが変わる（8にすると細かすぎておかしくなる）
        for (int i = 0; i < 2; ++i)
        {
            bloomAccum[0] += Get5x5GaussianBlur(
				tex3, smp, input.uv * uvSize + uvOfst, dx, dy, float4(uvOfst, uvOfst + uvSize));
            uvOfst.y += uvSize.y;
            uvSize *= 0.5;
        }
        uvSize = float2(1.0, 0.5);
        uvOfst = float2(0, 0);
        for (int i = 0; i < 2; ++i)
        {
            bloomAccum[1] += Get5x5GaussianBlur(
				tex4, smp, input.uv * uvSize + uvOfst, dx, dy, float4(uvOfst, uvOfst + uvSize));
            uvOfst.y += uvSize.y;
            uvSize *= 0.5;
        }

		//元の画像とぼかした高輝度の部分を足す
        return tex0.Sample(smp, input.uv) //通常テクスチャ
			+ saturate(bloomAccum[0]) / bloomPow //縮小ぼかし済み(/ で強さ調整)
			+ saturate(bloomAccum[1]) / bloomPow; //縮小ぼかし済み(/ で強さ調整)
    }
    
    if (isEffect)
    {
        return ret;
    }

    return RGBA;
}