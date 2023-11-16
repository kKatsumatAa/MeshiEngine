
#include"CommonHeader.hlsli"

//パッチ定数関数の出力値
struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[3]			: SV_TessFactor;//三角形の辺の分割量
	float InsideTessFactor			: SV_InsideTessFactor;//三角形内部の分割量
};

#define NUM_CONTROL_POINTS 3

HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(
	InputPatch<VSOutput2, NUM_CONTROL_POINTS> ip,
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
VSOutput2 main(
	InputPatch<VSOutput2, NUM_CONTROL_POINTS> ip,
	uint i : SV_OutputControlPointID,
	uint PatchID : SV_PrimitiveID )
{
    VSOutput2 Output;

	Output.svpos = ip[i].svpos;
	Output.tpos = ip[i].tpos;
	Output.worldpos = ip[i].worldpos;
	Output.binormal = ip[i].binormal;
	Output.normal = ip[i].normal;
	Output.tangent = ip[i].tangent;
	Output.uv = ip[i].uv;

	return Output;
}
