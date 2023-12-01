
#include"CommonHeader.hlsli"


float4 WavePos(float4 localPos, float3 normal, matrix world)
{
    float maxWidth = 0;
    
    //ワールド行列を位置と法線にかける
    float4 wPos = mul(world, localPos);
    float4 wNormal = mul(world, float4(normal, 0));
    
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

struct DSOutPutShadow
{
    float4 pos : SV_POSITION;
};

struct HSOutPutShadow
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
};

[domain("tri")]
DSOutPutShadow main(
	HS_CONSTANT_DATA_OUTPUT input, //パッチ定数関数からの出力値
	float3 domain : SV_DomainLocation, //パッチ内での位置
	const OutputPatch<HSOutPutShadow, NUM_CONTROL_POINTS> patch //ハルシェーダーから出力された制御点
)
{
    //メッシュの行列とワールドをかける    
    float4x4 worldL = mul(world, worldMesh);    
    
    DSOutPutShadow Output;
    
    //法線
    float3 normal = normalize(float3(
		patch[0].normal.xyz * domain.x + patch[1].normal.xyz * domain.y + patch[2].normal.xyz * domain.z));
    
    //システム用座標
    Output.pos = float4(
		patch[0].pos.xyz * domain.x + patch[1].pos.xyz * domain.y + patch[2].pos.xyz * domain.z, 1);
    Output.pos = WavePos(Output.pos, normal, worldL);
    Output.pos = mul(lightCamera, Output.pos);

    return Output;
}
