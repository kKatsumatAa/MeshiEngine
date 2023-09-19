


cbuffer ConstBufferEffectFlags : register(b0)
{
    //色
    float4 color;
	//ぼかし
    unsigned int isMultiTex;
	//ぼかし
    unsigned int isGaussian;
	//ガウシアンぼかし
    unsigned int isGaussian2;
    //ラジアルブラー
    unsigned int isRadialBlur;
    //ラジアル強さ
    float radialPow;
	//エンボス
    unsigned int isEmboss;
	//シャープネス
    unsigned int isSharpness;
	//諧調
    unsigned int isGradation;
	//アウトライン
    unsigned int isOutLine;
	//ビネット
    unsigned int isVignette;
	//樽状湾曲
    unsigned int isBarrelCurve;
    float barrelCurvePow;
	//走査線
    unsigned int isScanningLine;
	//グレースケール
    unsigned int isGrayScale;
    float grayScalePow;
	//ガラスフィルター
    unsigned int isGlassFilter;
	//モザイク
    unsigned int isMosaic;
	//ネガポジ反転
    unsigned int isNega;
	//ネガポジ反転
    unsigned int isRGBShift;
    float RGBShiftPow;
	//ブルーム
    unsigned int isBloom;
	//クロスフィルタ
    unsigned int isCrossFilter;
    //ブルームの強さ
    float bloomPow;
    //ノイズ
    uint isNoise;
    //被写界深度
    uint isDepthField;
    //フォーカス合わせる深度値
    float focusDepth;
	//フォーカスのスムースステップの幅の下限
    float nFocusWidth;
	//フォーカスのスムースステップの幅の上限
    float focusDiffPow;
	//時間
    uint time;
}

cbuffer PostEffect : register(b1)
{
    float4 bkweights[2];
}

//vsからpsに送る
struct Output
{
    float4 svpos : SV_POSITION;
    float2 uv : TEXCOORD;
};

//一枚目の一つ目(通常カラー)
Texture2D<float4> tex0 : register(t0);
//一枚目の二つ目(違うカラー（法線←まだ）)
Texture2D<float4> tex1 : register(t1);
//一枚目の三つ目(高輝度用)
Texture2D<float4> tex2 : register(t2);
//一枚目の四つ目(縮小バッファ高輝度)
Texture2D<float4> tex3 : register(t3);
//一枚目の5つ目(縮小バッファ高輝度2)
Texture2D<float4> tex4 : register(t4);
//6つ目(通常ぼかし（被写界深度用）)
Texture2D<float4> tex5 : register(t5);
//ガラスフィルター
Texture2D<float4> effectTex : register(t6);
//深度値(floatなのでR値のみ)
Texture2D<float> depthTex : register(t7);


//ぼかした後の画像を返す
float4 Get5x5GaussianBlur(Texture2D<float4> tex, SamplerState smp, float2 uv, float dx, float dy, float4 rect)
{
    float4 ret = tex.Sample(smp, uv);

    float l1 = -dx, l2 = -2 * dx;
    float r1 = dx, r2 = 2 * dx;
    float u1 = -dy, u2 = -2 * dy;
    float d1 = dy, d2 = 2 * dy;
    l1 = max(uv.x + l1, rect.x) - uv.x;
    l2 = max(uv.x + l2, rect.x) - uv.x;
    r1 = min(uv.x + r1, rect.z - dx) - uv.x;
    r2 = min(uv.x + r2, rect.z - dx) - uv.x;

    u1 = max(uv.y + u1, rect.y) - uv.y;
    u2 = max(uv.y + u2, rect.y) - uv.y;
    d1 = min(uv.y + d1, rect.w - dy) - uv.y;
    d2 = min(uv.y + d2, rect.w - dy) - uv.y;

    return float4((
		tex.Sample(smp, uv + float2(l2, u2)).rgb
		+ tex.Sample(smp, uv + float2(l1, u2)).rgb * 4
		+ tex.Sample(smp, uv + float2(0, u2)).rgb * 6
		+ tex.Sample(smp, uv + float2(r1, u2)).rgb * 4
		+ tex.Sample(smp, uv + float2(r2, u2)).rgb

		+ tex.Sample(smp, uv + float2(l2, u1)).rgb * 4
		+ tex.Sample(smp, uv + float2(l1, u1)).rgb * 16
		+ tex.Sample(smp, uv + float2(0, u1)).rgb * 24
		+ tex.Sample(smp, uv + float2(r1, u1)).rgb * 16
		+ tex.Sample(smp, uv + float2(r2, u1)).rgb * 4

		+ tex.Sample(smp, uv + float2(l2, 0)).rgb * 6
		+ tex.Sample(smp, uv + float2(l1, 0)).rgb * 24
		+ ret.rgb * 36
		+ tex.Sample(smp, uv + float2(r1, 0)).rgb * 24
		+ tex.Sample(smp, uv + float2(r2, 0)).rgb * 6

		+ tex.Sample(smp, uv + float2(l2, d1)).rgb * 4
		+ tex.Sample(smp, uv + float2(l1, d1)).rgb * 16
		+ tex.Sample(smp, uv + float2(0, d1)).rgb * 24
		+ tex.Sample(smp, uv + float2(r1, d1)).rgb * 16
		+ tex.Sample(smp, uv + float2(r2, d1)).rgb * 4

		+ tex.Sample(smp, uv + float2(l2, d2)).rgb
		+ tex.Sample(smp, uv + float2(l1, d2)).rgb * 4
		+ tex.Sample(smp, uv + float2(0, d2)).rgb * 6
		+ tex.Sample(smp, uv + float2(r1, d2)).rgb * 4
		+ tex.Sample(smp, uv + float2(r2, d2)).rgb
		) / 256.0f, ret.a);
}

//unityの方を
float4 GaussianTmp(float2 drawUV, float2 pickUV, float sigma)
{
    float d = distance(drawUV, pickUV);
    return exp(-(d * d) / (2.0 * sigma * sigma));
}

float4 Gaussian2(Texture2D<float4> tex, SamplerState smp, float2 uv)
{
    float totalWeight = 0, sigma = 0.03, stepWidth = 0.005;
    float4 col = float4(0, 0, 0, 0);

    for (float py = -sigma * 2; py <= sigma * 2; py += stepWidth)
    {
        for (float px = -sigma * 2; px <= sigma * 2; px += stepWidth)
        {
            float2 pickUV = uv + float2(px, py);
            float weight = GaussianTmp(uv, pickUV, sigma);
            col += tex.Sample(smp, pickUV) * weight; //重みを色にかける
            totalWeight += weight; //重みの合計値を計算
        }
    }

    col.rgb = col.rgb / totalWeight; //掛けた重み分結果から割る
    col.a = 1;
    return col;
}

float4 GaussianAngle(Texture2D<float4> tex, SamplerState smp, float angle, float2 uv)
{
    float totalWeight = 0, sigma = 0.005, stepWidth = 0.001;
    float color = float4(0, 0, 0, 0);
    float2 pickUV = float2(0, 0); //色を取得する座標
    float pickRange = 0.06; //ガウス関数式でいうシグマ
    float angleDeg = angle;
    float angleRad = angleDeg * 3.1415 / 180; //ぼかす角度

    for (float j = -pickRange * 2; j <= pickRange * 2; j += 0.005)
    {
        float x = cos(angleRad) * j; //角度から座標指定
        float y = sin(angleRad) * j;
        pickUV = uv + float2(x, y);

        float weight = GaussianTmp(uv, pickUV, pickRange); //自作のガウス関数で計算
        color += tex.Sample(smp, pickUV) * weight; //取得する色にweightをかける
        totalWeight += weight; //掛けるweightの合計値を控える
    }
    color = color / totalWeight; //足し合わせた色をweightの合計値で割る
    return color;
}

float4 GetRadialBlur(Texture2D<float4> tex, SamplerState smp, float2 center, float2 uv, float2 pixLength, float m_BlurPower)
{
    float4 Color[10];
   
   //ブラーの中心位置 ← 現在のテクセル位置
    float2 dir = center - float2(uv.r * center.r * 2.0,
    uv.g * center.g * 2.0);
   
   //距離を計算する
    float len = length(dir);
   
   //方向ベクトルの正規化し、１テクセル分の長さとなる方向ベクトルを計算する
    dir = normalize(dir) * pixLength;
   
   //m_BlurPower でボケ具合を調整する
   //距離を積算することにより、爆発の中心位置に近いほどブラーの影響が小さくなるようにする
    dir *= m_BlurPower * len;

   //合成する
    Color[0] = tex.Sample(smp, uv) * 0.19f;
    Color[1] = tex.Sample(smp, uv + dir) * 0.17f;
    Color[2] = tex.Sample(smp, uv + dir * 2.0f) * 0.15f;
    Color[3] = tex.Sample(smp, uv + dir * 3.0f) * 0.13f;
    Color[4] = tex.Sample(smp, uv + dir * 4.0f) * 0.11f;
    Color[5] = tex.Sample(smp, uv + dir * 5.0f) * 0.09f;
    Color[6] = tex.Sample(smp, uv + dir * 6.0f) * 0.07f;
    Color[7] = tex.Sample(smp, uv + dir * 7.0f) * 0.05f;
    Color[8] = tex.Sample(smp, uv + dir * 8.0f) * 0.03f;
    Color[9] = tex.Sample(smp, uv + dir * 9.0f) * 0.01f;
   
    return Color[0] + Color[1] + Color[2] + Color[3] + Color[4] + Color[5] + Color[6] + Color[7] + Color[8] + Color[9];
}

//ランダム
float random(float2 p, float extend = 1.0f);
float random(float2 p, float extend)
{
    return frac(sin(dot(p, float2(12.9898f, 78.233f))) * (43758.5453f + extend));
}

//ノイズ
float noise(float2 st, float extend = 1.0f);
float noise(float2 st, float extend)
{
    float2 p = floor(st);
    return random(p, extend); //
}