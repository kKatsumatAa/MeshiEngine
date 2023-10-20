#include "Particle.hlsli"

// ���_��
static const uint S_V_NUM = 3;

static const float PI = 3.1415f;

static const float angle = PI * 2.0f + PI * 2.0f / 3.0f * 2.0f;
static const float angle2 = PI * 2.0f;
static const float angle3 = PI * 2.0f + PI * 2.0f / 3.0f;

// �Z���^�[����̃I�t�Z�b�g�e�[�u��
static const float4 S_OFFSET_ARRAY[S_V_NUM] =
{
    float4(sin(angle), cos(angle), 0, 0), //����
    float4(0, cos(angle2), 0, 0), //��
    float4(sin(angle3), cos(angle3), 0, 0) //�E��
};

// UV�e�[�u���i���オ0,0�@�E����1,1�j
static const float2 S_UV_ARRAY[S_V_NUM] =
{
    float2(0, 1), // ����
	float2(0.5f, 0.5f), // ��
	float2(1, 1) // �E��
};

// �_�̓��͂���A3�p�`���o��
[maxvertexcount(S_V_NUM)]
void main(
	point VSOutput input[1] : SV_POSITION,
	inout TriangleStream<GSOutput> output
)
{
    GSOutput element;

    float4 offset = float4(0, 0, 0, 0);

    for (uint i = 0; i < S_V_NUM; i++)
    {
		// ���S����̃I�t�Z�b�g���X�P�[�����O
        offset = S_OFFSET_ARRAY[i] * input[0].scale;
        		// ���S����̃I�t�Z�b�g���r���{�[�h��]�i���f�����W�j
        offset = mul(matBillboard, offset);

        //�^���I�ɉ�]
         //���S���N�_�ɉ�]������
        element.svpos = input[0].pos +
                float4(rotate(offset.xyz, (input[0].rot.x + input[0].rot.y + input[0].rot.z) / 3.0f, input[0].rot.xyz), 1.0f) * input[0].scale;
		// �r���[�v���W�F�N�V�����ϊ�
        element.svpos = mul(mat, element.svpos);
        element.uv = S_UV_ARRAY[i];
        element.color = input[0].color;
        output.Append(element);
    }
}