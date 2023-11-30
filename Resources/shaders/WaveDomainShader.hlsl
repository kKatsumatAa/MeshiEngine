
#include"CommonHeader.hlsli"


float4 WavePos(float4 localPos, float3 normal, matrix world)
{
    float maxWidth = 0;
    
    //ワールド行列を位置と法線にかける
    float4 wPos = mul(world, localPos);
    float4 wNormal = mul(world, float4(normal,0));
    
    for (int i = 0; i < WAVE_NUM; i++)
    {
        //頂点の波の中心点からの距離
        float length = distance(wPos.xyz, waves[i].waveEpicenter);
        //波の高さの割合
        float t = max(1.0 - abs(waves[i].waveDistance - length) / waves[i].waveThickness.x, 0);
        
        maxWidth += max(waves[i].waveThickness.y * t, 0);
    }
    
    
    return float4(wPos.xyz + normalize(wNormal.xyz) * float3(maxWidth, maxWidth, maxWidth), 1.0f);
}

struct HS_CONSTANT_DATA_OUTPUT
{
    float EdgeTessFactor[3] : SV_TessFactor;
    float InsideTessFactor : SV_InsideTessFactor;
};

#define NUM_CONTROL_POINTS 3

[domain("tri")]
DSOutput main(
	HS_CONSTANT_DATA_OUTPUT input, //パッチ定数関数からの出力値
	float3 domain : SV_DomainLocation, //パッチ内での位置
	const OutputPatch<VSOutput2, NUM_CONTROL_POINTS> patch //ハルシェーダーから出力された制御点
)
{
    //メッシュの行列とワールドをかける    
    float4x4 worldL = mul(world, worldMesh);
    matrix viewWorld = mul(viewproj, worldL);
    
    
    DSOutput Output;
    
    //法線
    Output.normal = normalize(float3(
		patch[0].normal.xyz * domain.x + patch[1].normal.xyz * domain.y + patch[2].normal.xyz * domain.z));
    
    //システム用座標
    Output.svpos = float4(
		patch[0].svpos.xyz * domain.x + patch[1].svpos.xyz * domain.y + patch[2].svpos.xyz * domain.z, 1);
    Output.svpos = WavePos(Output.svpos, Output.normal, worldL);
    Output.svpos = mul(viewproj, Output.svpos);

    //ワールド座標
    Output.worldpos = float4(
		patch[0].worldpos.xyz * domain.x + patch[1].worldpos.xyz * domain.y + patch[2].worldpos.xyz * domain.z, 1);
    Output.worldpos = WavePos(Output.worldpos, Output.normal, worldL);
    //
    Output.tpos = float4(
		patch[0].tpos.xyz * domain.x + patch[1].tpos.xyz * domain.y + patch[2].tpos.xyz * domain.z, 1);
    Output.tpos = mul(lightCamera, Output.worldpos);
    
    Output.binormal = float3(
		patch[0].binormal.xyz * domain.x + patch[1].binormal.xyz * domain.y + patch[2].binormal.xyz * domain.z);
    Output.tangent = float3(
		patch[0].tangent.xyz * domain.x + patch[1].tangent.xyz * domain.y + patch[2].tangent.xyz * domain.z);
    Output.uv = float2(
		patch[0].uv.xy * domain.x + patch[1].uv.xy * domain.y + patch[2].uv.xy * domain.z);

    return Output;
}
