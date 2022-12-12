#include "Basic.hlsli"

Texture2D<float4> tex : register(t0); //0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0);      //0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[


float4 main(VSOutput input) : SV_TARGET
{
	//�e�N�X�`���}�b�s���O
	float4 texcolor = tex.Sample(smp, input.uv);

	//�V�F�[�f�B���O�ɂ��F
	float4 shadecolor;
	//����x
	const float shininess = 4.0f;
	//���_���王�_�ւ̕����x�N�g��
	float3 eyedir = normalize(cameraPos - input.worldpos.xyz);
	//���C�g�Ɍ������x�N�g���Ɩ@���̓���
	float3 dotlightnormal = dot(lightv, input.normal);
	//���ˌ��x�N�g��
	float3 reflect = normalize(-lightv + 2 * dotlightnormal * input.normal);
	//�����ˌ�
	float3 ambient = 0.06f;
	//�g�U���ˌ�
	float3 diffuse = dotlightnormal * 0.8f;
	//���ʔ��ˌ��isaturate->0~1�ւ̃N�����v�j
	float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * 0.1f;
	//�S�ĉ��Z
	shadecolor.rgb = (ambient + diffuse + specular) * lightcolor;
	shadecolor.a = 1.0f;

	//�V�F�[�f�B���O�ɂ��F�ŕ`��
	return shadecolor * texcolor * color;
}
