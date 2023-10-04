
#include "OBJShaderHeader.hlsli"

VSOutput main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD, 
float4 tangent : TANGENT, float4 binormal : BINORMAL)
{
    //���b�V���̍s��ƃ��[���h��������
    float4 wpos = mul(worldMesh, pos);
    wpos = mul(world, wpos);
    
    float4x4 worldL = mul(world, worldMesh);
    //���[���h�s��ɂ��X�P�[�����O�E��]��K�p

	//�s�N�Z���V�F�[�_�ɓn���l(���X�^���C�U�Ŋe�s�N�Z�����Ƃ̃��[���h�@���A���W���v�Z�����)
    VSOutput output; //�s�N�Z���V�F�[�_�ɓn���l
    output.svpos = mul(mul(viewproj, worldL), pos);
    output.worldpos = wpos;
    //���[�J���̖@���𑗂�A�s�N�Z���V�F�[�_�Ń��[���h�ϊ�
    output.normal = normalize(normal);
    output.tangent = normalize(tangent).rgb;
    output.binormal = normalize(cross(normalize(normal), normalize(tangent.rgb)));
    output.uv = uv;

    return output;
}