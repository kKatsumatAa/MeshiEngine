
#include "OBJShaderHeader.hlsli"

struct VSOutPutS
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
};


VSOutPutS main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD, uint4 boneIndices : BONEINDICES,
	float4 boneWeights : BONEWEIGHTS, float4 tangent : TANGENT, float4 binormal : BINORMAL)
{
	//ピクセルシェーダに渡す値(ラスタライザで各ピクセルごとのワールド法線、座標が計算される)
    VSOutPutS output; //ピクセルシェーダに渡す値
    output.pos = pos;
    output.normal = normal;

    return output;
}