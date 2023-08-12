
#include "CommonHeader.hlsli"

//頂点シェーダからピクセルシェーダーへのやり取りに使用する構造体
struct VSOutput
{
    float4 svpos : SV_POSITION; //システム用頂点座標
    float4 worldpos : POSITION; //ワールド座標
    float3 normal : NORMAL; //法線
    float2 uv : TEXCOORD; //uv座標
    float3 tangent : TANGENT; //法線の接線
    float3 binormal : BINORMAL; //法線の接線
};
