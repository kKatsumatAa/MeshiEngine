
#include "OBJShaderHeader.hlsli"

//スキニング後の頂点・法線が入る(uvはスキニングの影響受けないのでそのまま)
struct SkinOutput
{
    float4 pos;
    float3 normal;
    uint4 boneIndices; //ボーンの番号
    float4 boneWeights; //ボーンのスキンウェイト
};

//スキニング計算
SkinOutput ComputeSkin(SkinOutput input)
{
	//ゼロクリア
    SkinOutput output = (SkinOutput) 0;

    uint iBone; //計算するボーン情報
    float weight; //ボーンウェイト(重み)
    matrix m; //スキニング行列

	//ボーン0
    iBone = input.boneIndices.x;
    weight = input.boneWeights.x;
    m = matSkinning[iBone];
    output.pos += weight * mul(m, input.pos);
    output.normal += weight * mul((float3x3) m, input.normal);

	//ボーン1
    iBone = input.boneIndices.y;
    weight = input.boneWeights.y;
    m = matSkinning[iBone];
    output.pos += weight * mul(m, input.pos);
    output.normal += weight * mul((float3x3) m, input.normal);

	//ボーン2
    iBone = input.boneIndices.z;
    weight = input.boneWeights.z;
    m = matSkinning[iBone];
    output.pos += weight * mul(m, input.pos);
    output.normal += weight * mul((float3x3) m, input.normal);
    
	//ボーン3
    iBone = input.boneIndices.w;
    weight = input.boneWeights.w;
    m = matSkinning[iBone];
    output.pos += weight * mul(m, input.pos);
    output.normal += weight * mul((float3x3) m, input.normal);

    return output;
}

struct VSOutPutS
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
};


VSOutPutS main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD, uint4 boneIndices : BONEINDICES,
	float4 boneWeights : BONEWEIGHTS, float4 tangent : TANGENT, float4 binormal : BINORMAL)
{
    SkinOutput input = { pos, normal, boneIndices, boneWeights };
	//スキニング計算
    SkinOutput skinned = ComputeSkin(input);
    
    VSOutPutS o;
    
    o.pos = skinned.pos;
    o.normal = skinned.normal;
        
    return o;
}