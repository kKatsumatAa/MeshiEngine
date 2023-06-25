


cbuffer ConstBufferEffectFlags : register(b0)
{
	//ぼかし
	unsigned int isMultiTex;
	//ぼかし
	unsigned int isGaussian;
	//ガウシアンぼかし
	unsigned int isGaussian2;
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
	//走査線
	unsigned int isScanningLine;
	//グレースケール
	unsigned int isGrayScale;
	//ガラスフィルター
	unsigned int isGlassFilter;
	//モザイク
	unsigned int isMosaic;
	//ネガポジ反転
	unsigned int isNega;
	//ネガポジ反転
	unsigned int isRGBShift;
	//ブルーム
	unsigned int isBloom;
	//クロスフィルタ
	unsigned int isCrossFilter;
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
//ガラスフィルター
Texture2D<float4> effectTex : register(t5);


//ぼかした後の画像を返す
float4 Get5x5GaussianBlur(Texture2D<float4> tex, SamplerState smp, float2 uv, float dx, float dy, float4 rect) {
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
	float e = 2.71828182845904523536;
	return pow(e, -(d * d) / (2 * sigma * sigma));
}

float4 Gaussian2(Texture2D<float4> tex, SamplerState smp, float2 uv)
{
	float w, h, level;

	tex.GetDimensions(0, w, h, level);

	float dx = 1.0f / w;
	float dy = 1.0f / h;

	float4 RGBA = tex.Sample(smp, uv);
	float4 color = float4(0, 0, 0, 0);

	float4 ret = float4(0, 0, 0, 0);
	//ret += bkweights[0] * RGBA;
	for (int i = 1; i < 8; ++i) {
		ret += bkweights[i >> 2][i % 4] * tex.Sample(smp, clamp(uv + float2(0, dy * i), 0, 1.0f));
		ret += bkweights[i >> 2][i % 4] * tex.Sample(smp, clamp(uv + float2(0, -dy * i), 0, 1.0f));
	}
	//ret += bkweights[0] * RGBA;
	for (int i = 1; i < 8; ++i)
	{
		ret += bkweights[i >> 2][i % 4] * tex.Sample(smp, clamp(uv + float2(i * dx, 0), 0, 1.0f));
		ret += bkweights[i >> 2][i % 4] * tex.Sample(smp, clamp(uv + float2(-i * dx, 0), 0, 1.0f));
	}

	return ret;
}

float4 GaussianAngle(Texture2D<float4> tex, SamplerState smp, float angle, float2 uv)
{
	float totalWeight = 0, sigma = 0.005, stepWidth = 0.001;
	float color = float4(0, 0, 0, 0);
	float2 pickUV = float2(0, 0);//色を取得する座標
	float pickRange = 0.06;//ガウス関数式でいうシグマ
	float angleDeg = angle;
	float angleRad = angleDeg * 3.1415 / 180;//ぼかす角度

	for (float j = -pickRange * 2; j <= pickRange * 2; j += 0.005)
	{
		float x = cos(angleRad) * j;//角度から座標指定
		float y = sin(angleRad) * j;
		pickUV = uv + float2(x, y);

		float weight = GaussianTmp(uv, pickUV, pickRange);//自作のガウス関数で計算
		color += tex.Sample(smp, pickUV) * weight;//取得する色にweightをかける
		totalWeight += weight;//掛けるweightの合計値を控える
	}
	color = color / totalWeight;//足し合わせた色をweightの合計値で割る
	return color;
}