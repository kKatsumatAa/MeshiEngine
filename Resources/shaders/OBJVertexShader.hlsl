
#include "OBJShaderHeader.hlsli"

VSOutput main(float4 pos : POSITION, float3 normal : NORMAL,
	float2 uv : TEXCOORD)
{
	//�@���Ƀ��[���h�s��ɂ��X�P�[�����O�E��]��K�p
	float4 wnormal = normalize(mul(world, float4(normal, 0)));
	float4 wpos = mul(world, pos);

	//�s�N�Z���V�F�[�_�ɓn���l
	VSOutput output;//�s�N�Z���V�F�[�_�ɓn���l
	output.svpos = mul(mul(viewproj, world), pos);

	//�����ˌ�
	float3 ambient = m_ambient;
	//�g�U���ˌ�
	float3 diffuse = dot(lightv, wnormal.xyz) * m_diffuse;
	//����x
	const float shininess = 4.0f;
	//���_���王�_�ւ̕����x�N�g��
	float3 eyedir = normalize(cameraPos - wpos.xyz);
	//���ˌ��x�N�g��//��ڂ�lightdir��-���Ȃ�
	float3 reflect = normalize(-lightv + 2 * dot(lightv, wnormal.xyz) * wnormal.xyz);
	//���ʔ��ˌ��isaturate->0~1�ւ̃N�����v�j
	float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * m_specular;

	//�S�ĉ��Z����
	output.color.rgb = (ambient + diffuse + specular) * lightcolor;
	output.color.a = m_alpha;
	output.color.a = m_alpha;
	output.uv = uv;
	return output;
}