#include "Basic.hlsli"

struct VSOutPutS
{
    float4 pos : SV_POSITION;
};

//シャドウマップ用頂点座標変換
VSOutPutS main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD,
float4 tangent : TANGENT, float4 binormal : BINORMAL)
{
    VSOutPutS o;
    
    o.pos = mul(mul(lightCamera, world), pos);
        
    //ライトカメラを使う
    return o;
}