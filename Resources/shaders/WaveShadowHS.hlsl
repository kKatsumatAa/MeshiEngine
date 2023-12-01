
#include"CommonHeader.hlsli"

//パッチ定数関数の出力値
struct HS_CONSTANT_DATA_OUTPUT
{
    float EdgeTessFactor[3] : SV_TessFactor; //三角形の辺の分割量
    float InsideTessFactor : SV_InsideTessFactor; //三角形内部の分割量
};

#define NUM_CONTROL_POINTS 3

struct HSOutPutShadow
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
};

HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(
	InputPatch<HSOutPutShadow, NUM_CONTROL_POINTS> ip,
	uint PatchID : SV_PrimitiveID)
{
    HS_CONSTANT_DATA_OUTPUT Output;

    Output.EdgeTessFactor[0] =
		Output.EdgeTessFactor[1] =
		Output.EdgeTessFactor[2] =
		Output.InsideTessFactor = tessFactor;

    return Output;
}

[domain("tri")] // テッセレートするメッシュの形状を指定する
[partitioning("fractional_odd")] // パッチの分割に使用するアルゴリズムを指定する
[outputtopology("triangle_cw")] // メッシュの出力方法を指定する
[outputcontrolpoints(3)] // ハルシェーダーのコントロール ポイント フェーズがコールされる回数
[patchconstantfunc("CalcHSPatchConstants")] // 対応するハルシェーダーのパッチ定数フェーズのメイン関数
HSOutPutShadow main(
	InputPatch<HSOutPutShadow, NUM_CONTROL_POINTS> ip,
	uint i : SV_OutputControlPointID,
	uint PatchID : SV_PrimitiveID)
{
    HSOutPutShadow Output;

    Output.pos = ip[i].pos;
    Output.normal = ip[i].normal;

    return Output;
}
