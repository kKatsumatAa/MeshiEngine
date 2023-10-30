
#include "OBJShaderHeader.hlsli"

//�X�L�j���O��̒��_�E�@��������(uv�̓X�L�j���O�̉e���󂯂Ȃ��̂ł��̂܂�)
struct SkinOutput
{
    float4 pos;
    uint4 boneIndices; //�{�[���̔ԍ�
    float4 boneWeights; //�{�[���̃X�L���E�F�C�g
};

//�X�L�j���O�v�Z
SkinOutput ComputeSkinVertex(SkinOutput input)
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

	//�{�[��1
    iBone = input.boneIndices.y;
    weight = input.boneWeights.y;
    m = matSkinning[iBone];
    output.pos += weight * mul(m, input.pos);

	//�{�[��2
    iBone = input.boneIndices.z;
    weight = input.boneWeights.z;
    m = matSkinning[iBone];
    output.pos += weight * mul(m, input.pos);
    
	//�{�[��3
    iBone = input.boneIndices.w;
    weight = input.boneWeights.w;
    m = matSkinning[iBone];
    output.pos += weight * mul(m, input.pos);

    return output;
}

struct VSOutPutS
{
    float4 pos : SV_POSITION;
};


VSOutPutS main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD, uint4 boneIndices : BONEINDICES,
	float4 boneWeights : BONEWEIGHTS, float4 tangent : TANGENT, float4 binormal : BINORMAL)
{
    SkinOutput input = { pos, boneIndices, boneWeights };
	//�X�L�j���O�v�Z
    SkinOutput skinned = ComputeSkinVertex(input);
    
    float4x4 worldL = mul(world, worldMesh);    
    
    VSOutPutS o;
    
    o.pos = mul(mul(lightCamera, worldL), skinned.pos);
        
    //���C�g�J�������g��
    return o;
}