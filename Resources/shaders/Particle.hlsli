
#include"ParticleCommon.hlsli"

cbuffer cbuff0 : register(b0)
{
    matrix mat; // ビュープロジェクション行列
    matrix matBillboard; // ビルボード行列
};

// 頂点シェーダーからジオメトリシェーダーへのやり取りに使用する構造体
struct VSOutput
{
    float4 pos : POSITION; // 頂点座標
    float scale : TEXCOORD; // スケール
    float4 color : COLOR; // 色
    float3 rot : ROT; // 角度
};

//ジオメトリシェーダーからピクセルシェーダーへの出力
struct GSOutput
{
    float4 svpos : SV_POSITION; // システム用頂点座標
    float2 uv : TEXCOORD; // uv値
    float4 color : COLOR; // 色
};
