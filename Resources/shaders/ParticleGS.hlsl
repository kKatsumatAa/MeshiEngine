#include "Particle.hlsli"

// 四角形の頂点数
static const uint S_V_NUM = 4;

// センターからのオフセットテーブル
static const float4 S_OFFSET_ARRAY[S_V_NUM] =
{
	float4(-0.5f,-0.5f, 0, 0),	// 左下
	float4(-0.5f,+0.5f, 0, 0),	// 左上
	float4(+0.5f,-0.5f, 0, 0),	// 右下
	float4(+0.5f,+0.5f, 0, 0)	// 右上
};

// UVテーブル（左上が0,0　右下が1,1）
static const float2 S_UV_ARRAY[S_V_NUM] =
{
	float2(0, 1),	// 左下
	float2(0, 0),	// 左上
	float2(1, 1),	// 右下
	float2(1, 0) 	// 右上
};

// 点の入力から、四角形を出力
[maxvertexcount(S_V_NUM)]
void main(
	point VSOutput input[1] : SV_POSITION,
	inout TriangleStream< GSOutput > output
)
{
	GSOutput element;

	float4 offset = float4(0, 0, 0, 0);

	for (uint i = 0; i < S_V_NUM; i++) {
		// 中心からのオフセットをスケーリング
		offset = S_OFFSET_ARRAY[i] * input[0].scale;
		// 中心からのオフセットをビルボード回転（モデル座標）
		offset = mul(matBillboard, offset);
		// オフセット分ずらす（ワールド座標）
		element.svpos = input[0].pos + offset;
		// ビュープロジェクション変換
		element.svpos = mul(mat, element.svpos);
		element.uv = S_UV_ARRAY[i];
		element.color = input[0].color;
		output.Append(element);
	}
}