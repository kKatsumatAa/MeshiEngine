
#include "CommonHeader.hlsli"

cbuffer ConstBufferDataMaterial2 : register(b2)
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
    float3 normal : NORMAL; //ワールド座標
    float2 uv : TEXCOORD; //uv座標
};

//ボーンの最大数(Object.hと合わせる)
static const int S_MAX_BONES = 32;

//ボーンのスキニング行列
cbuffer skinning : register(b5)
{
    matrix matSkinning[S_MAX_BONES];
}