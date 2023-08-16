
#include "OBJShaderHeader.hlsli"

//�X�L�j���O��̒��_�E�@��������(uv�̓X�L�j���O�̉e���󂯂Ȃ��̂ł��̂܂�)
struct SkinOutput
{
    float4 pos;
    float3 normal;
    uint4 boneIndices; //�{�[���̔ԍ�
    float4 boneWeights; //�{�[���̃X�L���E�F�C�g
    float3 tangent;
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
    output.tangent += weight * mul((float3x3) m, input.tangent);

	//�{�[��1
    iBone = input.boneIndices.y;
    weight = input.boneWeights.y;
    m = matSkinning[iBone];
    output.pos += weight * mul(m, input.pos);
    output.normal += weight * mul((float3x3) m, input.normal);
    output.tangent += weight * mul((float3x3) m, input.tangent);

	//�{�[��2
    iBone = input.boneIndices.z;
    weight = input.boneWeights.z;
    m = matSkinning[iBone];
    output.pos += weight * mul(m, input.pos);
    output.normal += weight * mul((float3x3) m, input.normal);
    output.tangent += weight * mul((float3x3) m, input.tangent);
    
	//�{�[��3
    iBone = input.boneIndices.w;
    weight = input.boneWeights.w;
    m = matSkinning[iBone];
    output.pos += weight * mul(m, input.pos);
    output.normal += weight * mul((float3x3) m, input.normal);
    output.tangent += weight * mul((float3x3) m, input.tangent);

    return output;
}


VSOutput main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD, uint4 boneIndices : BONEINDICES,
	float4 boneWeights : BONEWEIGHTS, float4 tangent : TANGENT, float4 binormal : BINORMAL)
{
    SkinOutput input = { pos, normal, boneIndices, boneWeights, tangent.rgb };
	//�X�L�j���O�v�Z
    SkinOutput skinned = ComputeSkin(input);

    //���b�V���̍s��ƃ��[���h��������
    float4 wpos = mul(worldMesh, skinned.pos);
    wpos = mul(world, wpos);
    
    float4x4 worldL = mul(world, worldMesh);
    //���[���h�s��ɂ��X�P�[�����O�E��]��K�p

	//�s�N�Z���V�F�[�_�ɓn���l(���X�^���C�U�Ŋe�s�N�Z�����Ƃ̃��[���h�@���A���W���v�Z�����)
    VSOutput output; //�s�N�Z���V�F�[�_�ɓn���l
    output.svpos = mul(mul(viewproj, worldL), skinned.pos);
    output.worldpos = wpos;
    //���[�J���̖@���𑗂�A�s�N�Z���V�F�[�_�Ń��[���h�ϊ�
    output.normal = normalize(skinned.normal);
    output.tangent = normalize(skinned.tangent).rgb;
    output.binormal = normalize(cross(normalize(skinned.normal), normalize(skinned.tangent.rgb)));
    output.uv = uv;

    return output;
}