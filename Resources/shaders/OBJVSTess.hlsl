
#include "OBJShaderHeader.hlsli"

VSOutput2 main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD, uint4 boneIndices : BONEINDICES,
	float4 boneWeights : BONEWEIGHTS, float4 tangent : TANGENT, float4 binormal : BINORMAL)
{
	//�s�N�Z���V�F�[�_�ɓn���l(���X�^���C�U�Ŋe�s�N�Z�����Ƃ̃��[���h�@���A���W���v�Z�����)
    VSOutput2 output; //�s�N�Z���V�F�[�_�ɓn���l
    output.svpos = pos;
    output.worldpos = pos;
    output.tpos = pos;
    //���[�J���̖@���𑗂�A�s�N�Z���V�F�[�_�Ń��[���h�ϊ�
    output.normal = normalize(normal);
    output.tangent = normalize(tangent).rgb;
    output.binormal = normalize(cross(normalize(normal), normalize(tangent.rgb)));
    output.uv = uv;

    return output;
}