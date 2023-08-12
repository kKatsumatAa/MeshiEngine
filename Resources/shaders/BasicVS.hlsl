#include "Basic.hlsli"

VSOutput main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD, uint4 boneIndices : BONEINDICES,
	float4 boneWeights : BONEWEIGHTS, float4 tangent : TANGENT, float4 binormal : BINORMAL)
{
	//ワールド変換
    float4 wpos = mul(world, pos);

	//ピクセルシェーダに渡す値(ラスタライザで各ピクセルごとのワールド法線、座標が計算される)
    VSOutput output; //ピクセルシェーダに渡す値
    output.svpos = mul(mul(viewproj, world), pos);
    output.worldpos = wpos;
    //ローカルの法線を送り、ピクセルシェーダでワールド変換
    output.normal = normal;
    output.tangent = normalize(tangent.rgb);
    output.binormal = normalize(cross(output.normal, output.tangent));
    output.uv = uv;

    return output;
}