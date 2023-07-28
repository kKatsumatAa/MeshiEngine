#include "Particle.hlsli"

// 頂点数
static const uint S_V_NUM = 3;

static const float PI = 3.1415f;

static const float angle = PI * 2.0f + PI * 2.0f / 3.0f * 2.0f;
static const float angle2 = PI * 2.0f;
static const float angle3 = PI * 2.0f + PI * 2.0f / 3.0f;

// センターからのオフセットテーブル
static const float4 S_OFFSET_ARRAY[S_V_NUM] =
{
    float4(sin(angle), cos(angle), 0, 0), //左下
    float4(0, cos(angle2), 0, 0), //上
    float4(sin(angle3), cos(angle3), 0, 0) //右下
};

// UVテーブル（左上が0,0　右下が1,1）
static const float2 S_UV_ARRAY[S_V_NUM] =
{
    float2(0, 1), // 左下
	float2(0.5f, 0.5f), // 上
	float2(1, 1) // 右下
};

// 点の入力から、3角形を出力
[maxvertexcount(S_V_NUM)]
void main(
	point VSOutput input[1] : SV_POSITION,
	inout TriangleStream<GSOutput> output
)
{
    GSOutput element;

    float4 offset = float4(0, 0, 0, 0);

    for (uint i = 0; i < S_V_NUM; i++)
    {
		// 中心からのオフセットをスケーリング
        offset = S_OFFSET_ARRAY[i] * input[0].scale;
        		// 中心からのオフセットをビルボード回転（モデル座標）
        offset = mul(matBillboard, offset);

        //疑似的に回転
        offset.r = sin(input[0].rot.r) * offset.r;
        offset.g = cos(input[0].rot.g) * offset.g;
        offset.b = -sin(input[0].rot.b) * offset.b;
        offset.a = -cos(input[0].rot.b) * offset.a;
		// オフセット分ずらす（ワールド座標）
        element.svpos = input[0].pos + offset;
		// ビュープロジェクション変換
        element.svpos = mul(mat, element.svpos);
        element.uv = S_UV_ARRAY[i];
        element.color = input[0].color;
        element.rot = input[0].rot;
        output.Append(element);
    }
}