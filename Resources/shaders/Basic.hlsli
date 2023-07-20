
#include "CommonHeader.hlsli"

//頂点シェーダからピクセルシェーダーへのやり取りに使用する構造体
struct VSOutput
{
    float4 svpos : SV_POSITION; //システム用頂点座標
    float4 worldpos : POSITION; //ワールド座標
    float3 normal : NORMAL; //ワールド座標
    float2 uv : TEXCOORD; //uv座標
};
