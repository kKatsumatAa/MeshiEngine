
#include "OBJShaderHeader.hlsli"

struct VSOutPutS
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
};


VSOutPutS main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD, uint4 boneIndices : BONEINDICES,
	float4 boneWeights : BONEWEIGHTS, float4 tangent : TANGENT, float4 binormal : BINORMAL)
{
	//�s�N�Z���V�F�[�_�ɓn���l(���X�^���C�U�Ŋe�s�N�Z�����Ƃ̃��[���h�@���A���W���v�Z�����)
    VSOutPutS output; //�s�N�Z���V�F�[�_�ɓn���l
    output.pos = pos;
    output.normal = normal;

    return output;
}