
#include "CommonHeader.hlsli"

cbuffer ConstBufferDataMaterial2 : register(b2)
{
    float3 m_ambient : packoffset(c0); //�A���r�G���g
    float3 m_diffuse : packoffset(c1); //�f�B�t���[�Y
    float3 m_specular : packoffset(c2); //�X�y�L�����[
    float m_alpha : packoffset(c2.w); //�A���t�@
}

//���_�V�F�[�_����s�N�Z���V�F�[�_�[�ւ̂����Ɏg�p����\����
struct VSOutput
{
    float4 svpos : SV_POSITION; //�V�X�e���p���_���W
    float4 worldpos : POSITION; //���[���h���W
    float3 normal : NORMAL; //�@��
    float2 uv : TEXCOORD1; //uv���W
    float3 tangent : TANGENT; //�@���̐ڐ�
    float3 binormal : BINORMAL; //�]�@��
};

//�{�[���̍ő吔(Object.h�ƍ��킹��)
static const int S_MAX_BONES = 90;

//�{�[���̃X�L�j���O�s��
cbuffer skinning : register(b5)
{
    matrix matSkinning[S_MAX_BONES];
}