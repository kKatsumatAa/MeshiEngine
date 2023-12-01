
#include "OBJShaderHeader.hlsli"

//�X�L�j���O��̒��_�E�@��������(uv�̓X�L�j���O�̉e���󂯂Ȃ��̂ł��̂܂�)
struct SkinOutput
{
    float4 pos;
    float3 normal;
    uint4 boneIndices; //�{�[���̔ԍ�
    float4 boneWeights; //�{�[���̃X�L���E�F�C�g
};

//�X�L�j���O�v�Z
SkinOutput ComputeSkin(SkinOutput input)
{
	//�[���N���A
    SkinOutput output = (SkinOutput) 0;

    uint iBone; //�v�Z����{�[�����
    float weight; //�{�[���E�F�C�g(�d��)
    matrix m; //�X�L�j���O�s��

	//�{�[��0
    iBone = input.boneIndices.x;
    weight = input.boneWeights.x;
    m = matSkinning[iBone];
    output.pos += weight * mul(m, input.pos);
    output.normal += weight * mul((float3x3) m, input.normal);

	//�{�[��1
    iBone = input.boneIndices.y;
    weight = input.boneWeights.y;
    m = matSkinning[iBone];
    output.pos += weight * mul(m, input.pos);
    output.normal += weight * mul((float3x3) m, input.normal);

	//�{�[��2
    iBone = input.boneIndices.z;
    weight = input.boneWeights.z;
    m = matSkinning[iBone];
    output.pos += weight * mul(m, input.pos);
    output.normal += weight * mul((float3x3) m, input.normal);
    
	//�{�[��3
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
	//�X�L�j���O�v�Z
    SkinOutput skinned = ComputeSkin(input);
    
    VSOutPutS o;
    
    o.pos = skinned.pos;
    o.normal = skinned.normal;
        
    return o;
}