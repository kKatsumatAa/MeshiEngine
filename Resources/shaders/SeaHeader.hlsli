/*
 * "Seascape" by Alexander Alekseev aka TDM - 2014
 * License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
 */

#include"PeraShaderHeader.hlsli"

// math
float3x3 fromEuler(float3 ang)
{
    float2 a1 = float2(sin(ang.z), -cos(ang.z));
    float2 a2 = float2(sin(ang.x), cos(ang.x));
    float2 a3 = float2(sin(ang.y), cos(ang.y));
    float3x3 m;
    m[0] = float3(a1.y * a3.y + a1.x * a2.x * a3.x, a1.y * a2.x * a3.x + a3.y * a1.x, -a2.y * a3.x);
    m[1] = float3(-a2.y * a1.x, a1.y * a2.y, a2.x);
    m[2] = float3(a3.y * a1.x * a2.x + a1.y * a3.x, a1.x * a3.x - a1.y * a3.y * a2.x, a2.y * a3.y);
    return m;
}

//-------------------------------------

static const int NUM_STEPS = 8;
static const float PI = 3.141592;
static const float EPSILON = 1e-3;
static const float TIME_EXTEND = 0.04f;
static const float2 IRESO = { 1280.0f, 720.0f };

//法線にかける小さい値
#define EPSILON_NRM (0.1 / IRESO.x)
#define SEA_TIME (1.0 + time*TIME_EXTEND * SEA_SPEED*seaTimeExtend)

// sea
static const int ITER_GEOMETRY = 3; //octave
static const int ITER_FRAGMENT = 5; //詳細な値を求めるためのoctave
static const float SEA_HEIGHT = 0.6;
static const float SEA_CHOPPY = 4.0 * seaTimeExtend;
static const float SEA_SPEED = 0.8;
static const float SEA_FREQ = 0.16;
static const float3 SEA_BASE = float3(0.1, 0.19, 0.22);
static const float3 SEA_WATER_COLOR = float3(0.8, 0.9, 0.6);
static const float2x2 octave_m = float2x2(1.6, 1.2, -1.2, 1.6);

// Get random value.
//疑似ランダムなので同じ値を入れたら同じ値が返ってくる
float hash(float2 p)
{
    float h = dot(p, float2(127.1, 311.7));
    return frac(sin(h) * 43758.5453123);
}

// Get Noise.
//バリューノイズ(疑似)
float noiseS(float2 p)
{
    //整数と少数に分ける
    float2 i = floor(p);
    float2 f = frac(p);

    // Get each grid vertices.
    // +---+---+
    // | a | b |
    // +---+---+
    // | c | d |
    // +---+---+
    //整数部分を使ってグリッドを決定
    float a = hash(i + float2(0.0, 0.0));
    float b = hash(i + float2(1.0, 0.0));
    float c = hash(i + float2(0.0, 1.0));
    float d = hash(i + float2(1.0, 1.0));
    
    // u = -2.0f^3 + 3.0f^2
    //小数部分でグリッドを補完
    float2 u = f * f * (3.0 - 2.0 * f);

    // Interpolate grid parameters with x and y.
    //a,b c,dを横方向に補間、その結果を縦方向に補間
    float result = lerp(lerp(a, b, u.x),
                        lerp(c, d, u.x), u.y);

    // Normalized to '-1 - 1'.
    //-1.0~1.0に補正している
    return (2.0 * result) - 1.0;
}

// lighting
float diffuse(float3 n, float3 l, float p)
{
    return pow(dot(n, l) * 0.4 + 0.6, p);
}

float specular(float3 n, float3 l, float3 e, float s)
{
    float nrm = (s + 8.0) / (PI * 8.0);
    return pow(max(dot(reflect(e, n), l), 0.0), s) * nrm;
}

// Get sky color by 'eye' position.
// So, The color changed smoothly by eye level.
float3 getSkyColor(float3 e)
{
    //カメラの位置のy座標のみを利用
    e.y = max(e.y, 0.0);
    //水平線近くは白くして、上の方に行くにしたがい青く
    float r = pow(1.0 - e.y, 2.0);
    float g = 1.0 - e.y;
    float b = 0.6 + (1.0 - e.y) * 0.4;
    return float3(r, g, b) * seaSkyCol;
}

// Get sea wave octave.
float sea_octave(float2 uv, float choppy)
{
    //ノイズを加算
    uv += noiseS(uv);
    //波を表現するためにsin,cos
    float2 wv = 1.0 - abs(sin(uv));
    float2 swv = abs(cos(uv));
    //線形補間して合成
    wv = lerp(wv, swv, wv);
    //複雑に合成(wv.xを返しても同じ感じ)
    return pow(1.0 - pow(wv.x * wv.y, 0.65), choppy);
}

// p is ray position.
float map(float3 p)
{
    float freq = SEA_FREQ; // 周波数
    float amp = SEA_HEIGHT; // 振幅
    float choppy = SEA_CHOPPY; // 波乱

    // XZ平面による計算
    float2 uv = p.xz;

    float d, h = 0.0;

    // ITER_GEOMETRY = 3
    // ここで「フラクタルブラウン運動(同じ関数に微妙に違う引数を持たせて何回も)」によるノイズの計算を行っている
    //(ノイズを複数重ねることで実現
    //→周波数を一定の割合で増加させる（lacunarity）と同時に振幅を減らしながら（gain）ノイズを（octaveの数だけ）繰り返し重ねる)
    for (int i = 0; i < ITER_GEOMETRY; i++)
    {
        // SEA_SPEED = 0.8
        // 単純に、iTime（時間経過）によってUV値を微妙にずらしてアニメーションさせている
        // iTimeのみを指定してもほぼ同じアニメーションになる
        //
        // SEA_TIMEのプラス分とマイナス分を足して振幅を大きくしている・・・？
        d = sea_octave((uv + SEA_TIME) * freq, choppy) * seaTimeExtend * 2.0f;
        d += sea_octave((uv - SEA_TIME) * freq, choppy) * seaTimeExtend * 2.0f;

        h += d * amp;

        // octave_m = mat2(1.6, 1.2, -1.2, 1.6);
        // uv値を回転させている風。これをなくすと平坦な海になる
        uv = mul(octave_m, uv);

        // fbm関数として、振幅を0.2倍、周波数を2.0倍して次の計算を行う
        freq *= 2.0;
        amp *= 0.2;

        // choppyを翻訳すると「波瀾」という意味
        // これを小さくすると海が「おとなしく」なる
        choppy = lerp(choppy, 1.0, 0.2);
    }

    // 最後に、求まった高さ`h`を、現在のレイの高さから引いたものを「波の高さ」としている
    return p.y - h;
}

//map()と処理は一緒でイテレーション回数が多いだけ
//(イテレーション回数がコンパイル時に決まってなきゃいけないため分けてる)
float map_detailed(float3 p)
{
    float freq = SEA_FREQ; // 周波数
    float amp = SEA_HEIGHT; // 振幅
    float choppy = SEA_CHOPPY; // 波乱

    float2 uv = p.xz;

    float d, h = 0.0;

    // ITER_FRAGMENT = 5
    for (int i = 0; i < ITER_FRAGMENT; i++)
    {
        d = sea_octave((uv + SEA_TIME) * freq, choppy);
        d += sea_octave((uv - SEA_TIME) * freq, choppy);
        h += d * amp;
        uv = mul(octave_m, uv);
        freq *= 2.0;
        amp *= 0.2;
        choppy = lerp(choppy, 1.0, 0.2);
    }

    return p.y - h;
}

// p = ray position
// n = surface normal
// l = light
// eye = eye
// dist = ray marching distance
float3 getSeaColor(float3 p, float3 n, float3 l, float3 eye, float3 dist)
{
    //フレネルの式(視線方向と面の向きで透過率と反射率が変化
    //→法線と視線方向へのベクトルの内積が一致する程仲が良く見え、反対なほど景色が映る)
    float fresnel = clamp(1.0 - dot(n, -eye), 0.0, 1.0);
    fresnel = pow(fresnel, 3.0) * 0.65;

    //屈折先の海の色、反射先の空の色を計算
    float3 reflected = getSkyColor(reflect(eye, n));
    float3 refracted = SEA_BASE + diffuse(n, l, 80.0) * SEA_WATER_COLOR * 0.12;

    //線形補間
    float3 colorL = lerp(refracted, reflected, fresnel);

    //減衰
    float atten = max(1.0 - dot(dist, dist) * 0.001, 0.0);
    colorL += SEA_WATER_COLOR * (p.y - SEA_HEIGHT) * 0.18 * atten;
    
    //鏡面反射
    float s = specular(n, l, eye, 60.0);
    colorL += float3(s, s, s);

    return colorL * seaCol;
}

// tracing
float3 getNormal(float3 p, float eps)
{
    //レイの当たった波の位置と
    float3 n;
    n.y = map_detailed(p);
    n.x = map_detailed(float3(p.x + eps, p.y, p.z)) - n.y;
    n.z = map_detailed(float3(p.x, p.y, p.z + eps)) - n.y;
    n.y = eps;
    return normalize(n);
}

// Get Height Map
float3 heightMapTracing(float3 ori, float3 dir)
{
    //レイの始点までの長さ
    float tm = 0.0;
    //レイの終点までの長さ
    float tx = 1000.0;

    float3 p = 0;
    
    // 1000.0m先のヘイトマップの距離（引数の位置から真下の波→引数の位置への距離）
    float hx = map(ori + dir * tx);

    // 1000.0m遠くのheightmapを計算し、0以上なら水平面より上で、海にレイが当たらないので抜ける
    //終点は海より下にあること前提なので
    if (hx > 0.0)
    {
        p = float3(0, 0, 0);
        return tx;
    }

    //カメラの位置のヘイトマップの距離（引数の位置から真下の波→引数の位置への距離）
    float hm = map(ori + dir * tm);
    float tmid = 0.0;

    // NUM_STEPS = 8
    //レイの開始地点から1000.0m先を両端点として計算
    for (int i = 0; i < NUM_STEPS; i++)
    {
        // hm（開始点のヘイトマップ距離）とhx（終点の〃）の比率に応じてどの位置のレイをサンプルするかを決めるための「重み」を計算
        float f = hm / (hm - hx);
        //レイの長さを線形補間で出す
        tmid = lerp(tm, tx, f);
        p = ori + dir * tmid;
        //ヘイトマップの距離
        float hmid = map(p);

        // サンプリング位置の高さがマイナス距離の場合は`hx`, `tx`(終点)を更新する
        //hxは負の値
        if (hmid < 0.0)
        {
            tx = tmid; //終点のレイの長さ
            hx = hmid; //終点のヘイトマップの距離
        }
        // そうでない場合は、`hm`, `tm`（始点）を更新する
        //hmは正の値
        else
        {
            tm = tmid; //始点のレイの長さ
            hm = hmid; //始点のヘイトマップの距離
        }
    }

    return p;
}

// main
float4 mainImage(float2 fragCoord)
{
    //解像度からuv座標に変換
    float2 uv = fragCoord.xy / (IRESO.xy * resoulution);
    uv = uv * 2.0 - 1.0;

    const float3 light = normalize(float3(0.0, 1.0, 0.8));

    // 回転行列
    float3x3 rotM = fromEuler(seaDirRot * seaDirRotExtend);
    
    //カメラの位置
    float3 ori = float3(seaCameraPos.z, seaCameraPos.y, seaCameraPos.x);
    //uv座標を使って方向ベクトルを計算
    float3 dir = normalize(float3(uv.x, uv.y, 4.0f));
    //方向ベクトルを回転
    dir = mul(rotM, dir);

    // レイの当たった場所
    float3 p;
    p = heightMapTracing(ori, dir);

    //カメラからの距離
    float3 dist = p - ori;
    //法線
    float3 n = getNormal(p, dot(dist, dist) * EPSILON_NRM);

    //海と空の色
    float3 sky = getSkyColor(dir);
    float3 sea = getSeaColor(p, n, light, dir, dist);

    // 方向ベクトルのyを使って係数計算
    float t = pow(smoothstep(0.0, -0.05, dir.y), 0.3);
    //海と空の色を線形補間
    float3 colorL = lerp(sky, sea, t);

    
    //仮でガラスフィルター用の画像を使って溶岩表現
    colorL = float4(colorL, 1.0f);
    
    if(isSeaImageEffect)
    {
        colorL *= max(float4(effectTex.Sample(smp, abs(frac(float2(-dist.x - time * 0.1, -dist.z) / 7.0))).rgb * 3.0f, 1.0f),0.01);
    }
    
    // post
    return float4(colorL, 1.0);
}