
#include "CommonHeader.hlsli"

//メッシュの行列
cbuffer ConstBufferDataTransformMesh : register(b2)
{
    matrix viewprojMesh; //ビュープロジェクション行列
    matrix worldMesh; //ワールド行列
    float3 cameraPosMesh; //カメラ座標（ワールド座標）
}

cbuffer ConstBufferDataMaterial2 : register(b3)
{
    float3 m_ambient : packoffset(c0); //アンビエント
    float3 m_diffuse : packoffset(c1); //ディフューズ
    float3 m_specular : packoffset(c2); //スペキュラー
    float m_alpha : packoffset(c2.w); //アルファ
}

//頂点シェーダからピクセルシェーダーへのやり取りに使用する構造体
struct VSOutput
{
    float4 svpos : SV_POSITION; //システム用頂点座標
    float4 worldpos : POSITION; //ワールド座標
    float3 normal : NORMAL; //法線
    float2 uv : TEXCOORD1; //uv座標
    float3 tangent : TANGENT; //法線の接線
    float3 binormal : BINORMAL; //従法線
};

//ボーンの最大数(Object.hと合わせる)
static const int S_MAX_BONES = 90;

//ボーンのスキニング行列
cbuffer skinning : register(b6)
{
    matrix matSkinning[S_MAX_BONES];
}