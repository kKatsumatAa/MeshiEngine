
#include "CommonHeader.hlsli"

//���_�V�F�[�_����s�N�Z���V�F�[�_�[�ւ̂����Ɏg�p����\����
struct VSOutput
{
    float4 svpos : SV_POSITION; //�V�X�e���p���_���W
    float4 worldpos : POSITION; //���[���h���W
    float3 normal : NORMAL; //�@��
    float2 uv : TEXCOORD; //uv���W
    float3 tangent : TANGENT; //�@���̐ڐ�
    float3 binormal : BINORMAL; //�@���̐ڐ�
};
