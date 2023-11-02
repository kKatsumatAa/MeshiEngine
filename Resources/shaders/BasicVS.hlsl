#include "CommonHeader.hlsli"

VSOutput main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD,
float4 tangent : TANGENT, float4 binormal : BINORMAL)
{
	//���[���h�ϊ�
    float4 wpos = mul(world, pos);

	//�s�N�Z���V�F�[�_�ɓn���l(���X�^���C�U�Ŋe�s�N�Z�����Ƃ̃��[���h�@���A���W���v�Z�����)
    VSOutput output; //�s�N�Z���V�F�[�_�ɓn���l
    output.svpos = mul(mul(viewproj, world), pos);
    output.worldpos = wpos;
    output.tpos = mul(lightCamera, wpos);
    //���[�J���̖@���𑗂�A�s�N�Z���V�F�[�_�Ń��[���h�ϊ�
    output.normal = normal;
    output.tangent = normalize(tangent.rgb);
    output.binormal = normalize(cross(output.normal, output.tangent));
    output.uv = uv;

    return output;
}