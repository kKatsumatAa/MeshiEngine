
//3D�ϊ��s��
cbuffer ConstBufferDataTransform : register(b1)
{
	matrix mat;//3D�ϊ��s��
}

cbuffer ConstBufferDataMaterial2 : register(b2)
{
	float3 m_ambient  : packoffset(c0);//�A���r�G���g
	float3 m_diffuse  : packoffset(c1);//�f�B�t���[�Y
	float3 m_specular : packoffset(c2);//�X�y�L�����[
	float  m_alpha    : packoffset(c2.w);//�A���t�@
}

//���_�V�F�[�_����s�N�Z���V�F�[�_�[�ւ̂����Ɏg�p����\����
struct VSOutput
{
	float4 svpos  : SV_POSITION;//�V�X�e���p���_���W
	float4 color  : COLOR;//�F
	float2 uv     : TEXCOORD;//uv���W
};