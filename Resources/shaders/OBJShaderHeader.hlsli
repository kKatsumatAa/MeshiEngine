
#include "CommonHeader.hlsli"

cbuffer ConstBufferDataMaterial2 : register(b3)
{
    float3 m_ambient : packoffset(c0); //アンビエント
    float3 m_diffuse : packoffset(c1); //ディフューズ
    float3 m_specular : packoffset(c2); //スペキュラー
    float m_alpha : packoffset(c2.w); //アルファ
}

//ボーンの最大数(Object.hと合わせる)
static const int S_MAX_BONES = 90;

//ボーンのスキニング行列
cbuffer skinning : register(b6)
{
    matrix matSkinning[S_MAX_BONES];
}