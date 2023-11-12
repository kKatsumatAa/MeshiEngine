
#include"CommonHeader.hlsli"

struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[3]			: SV_TessFactor;
	float InsideTessFactor			: SV_InsideTessFactor;
};

#define NUM_CONTROL_POINTS 3

[domain("tri")]
VSOutput main(
	HS_CONSTANT_DATA_OUTPUT input,//パッチ定数関数からの出力値
	float3 domain : SV_DomainLocation,//パッチ内での位置
	const OutputPatch<VSOutput, NUM_CONTROL_POINTS> patch//ハルシェーダーから出力された制御点
)
{
    VSOutput Output;

	Output.svpos = float4(
		patch[0].svpos.xyz * domain.x + patch[1].svpos.xyz * domain.y + patch[2].svpos.xyz * domain.z, 1);
    Output.tpos = float4(
		patch[0].tpos.xyz * domain.x + patch[1].tpos.xyz * domain.y + patch[2].tpos.xyz * domain.z, 1);
	Output.worldpos = float4(
		patch[0].worldpos.xyz * domain.x + patch[1].worldpos.xyz * domain.y + patch[2].worldpos.xyz * domain.z, 1);
	Output.binormal = float3(
		patch[0].binormal.xyz * domain.x + patch[1].binormal.xyz * domain.y + patch[2].binormal.xyz * domain.z);
	Output.normal = float3(
		patch[0].normal.xyz * domain.x + patch[1].normal.xyz * domain.y + patch[2].normal.xyz * domain.z);
	Output.tangent = float3(
		patch[0].tangent.xyz * domain.x + patch[1].tangent.xyz * domain.y + patch[2].tangent.xyz * domain.z);
	Output.uv = float2(
		patch[0].uv.xy * domain.x + patch[1].uv.xy * domain.y + patch[2].uv.xy * domain.z);

	return Output;
}
