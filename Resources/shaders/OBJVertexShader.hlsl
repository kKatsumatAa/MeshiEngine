
#include "OBJShaderHeader.hlsli"

VSOutput main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD, 
float4 tangent : TANGENT, float4 binormal : BINORMAL)
{
    //メッシュの行列とワールドをかける
    float4 wpos = mul(worldMesh, pos);
    wpos = mul(world, wpos);
    
    float4x4 worldL = mul(world, worldMesh);
    //ワールド行列によるスケーリング・回転を適用

	//ピクセルシェーダに渡す値(ラスタライザで各ピクセルごとのワールド法線、座標が計算される)
    VSOutput output; //ピクセルシェーダに渡す値
    output.svpos = mul(mul(viewproj, worldL), pos);
    output.worldpos = wpos;
    //ローカルの法線を送り、ピクセルシェーダでワールド変換
    output.normal = normalize(normal);
    output.tangent = normalize(tangent).rgb;
    output.binormal = normalize(cross(normalize(normal), normalize(tangent.rgb)));
    output.uv = uv;

    return output;
}