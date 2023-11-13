
#include "OBJShaderHeader.hlsli"

VSOutput2 main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD, uint4 boneIndices : BONEINDICES,
	float4 boneWeights : BONEWEIGHTS, float4 tangent : TANGENT, float4 binormal : BINORMAL)
{
	//ピクセルシェーダに渡す値(ラスタライザで各ピクセルごとのワールド法線、座標が計算される)
    VSOutput2 output; //ピクセルシェーダに渡す値
    output.svpos = pos;
    output.worldpos = pos;
    output.tpos = pos;
    //ローカルの法線を送り、ピクセルシェーダでワールド変換
    output.normal = normalize(normal);
    output.tangent = normalize(tangent).rgb;
    output.binormal = normalize(cross(normalize(normal), normalize(tangent.rgb)));
    output.uv = uv;

    return output;
}