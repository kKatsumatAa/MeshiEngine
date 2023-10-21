
#include"ParticleCommon.hlsli"

// ���_�V�F�[�_�[����W�I���g���V�F�[�_�[�ւ̂����Ɏg�p����\����
struct VSOutput
{
    float4 pos : POSITION; // ���_���W
    float scale : TEXCOORD; // �X�P�[��
    float4 color : COLOR; // �F
    float3 rot : ROT; // �p�x
};

//�W�I���g���V�F�[�_�[����s�N�Z���V�F�[�_�[�ւ̏o��
struct GSOutput
{
    float4 svpos : SV_POSITION; // �V�X�e���p���_���W
    float4 worldpos : POSITION; // ���[���h���_���W
    float2 uv : TEXCOORD; // uv�l
    float4 color : COLOR; // �F
    float3 normal : NORMAL;//�@��
};

//----------------------------------------------------------------------------------------------
// �_�����̐�
static const int S_POINTLIGHT_NUM = 12;
struct PointLight
{
    float3 lightpos; //���C�g�̍��W
    float3 lightcolor; //���C�g�̐F
    float3 lightatten; //���C�g�̋��������W��
    uint active; //�L����
};

// ���s�����̐�
static const int S_DIRLIGHT_NUM = 3;
struct DirLight
{
    float3 lightv; // ���C�g�ւ̕����̒P�ʃx�N�g��
    float3 lightcolor; // ���C�g�̐F(RGB)
    uint active; //�L����
};


static const int S_SPOTLIGHT_NUM = 3;
struct SpotLight
{
    float3 lightv; // ���C�g�ւ̕����̒P�ʃx�N�g��
    float3 lightpos; // ���C�g�̈ʒu
    float3 lightcolor; // ���C�g�̐F(RGB)
    float3 lightatten; // ���C�g�̌����W��
    float2 lightfactoranglecos; //���C�g�����p�x�̃R�T�C��
    uint active; //�L����
};

//�ۉe
static const int S_CIRCLESHADOW_NUM = 1;
struct CircleShadow
{
    float3 dir; //���e�����̋t�x�N�g��
    float3 casterPos; //�L���X�^�[���W
    float distanceCasterLight; //�L���X�^�[�ƃ��C�g�̋���
    float3 atten; //���������W��
    float2 factorAngleCos; //�����p�x�̃R�T�C��
    uint active;
};

//--------------------
cbuffer cbuff0 : register(b0)
{
    matrix mat; // �r���[�v���W�F�N�V�����s��
    matrix matBillboard; // �r���{�[�h�s��
};

//�J�����ʒu
cbuffer ConstBufferDataTransform : register(b1)
{
    float3 cameraPos; //�J�������W�i���[���h���W�j
}

//LightManager��ConstBuffer�Ɠ����^��錾
cbuffer ConstBufferDataMaterial3 : register(b2)
{
    float3 ambientColor;
    float3 diffuseColor;
    float3 specularColor;
    DirLight dirLights[S_DIRLIGHT_NUM];
    PointLight pointLights[S_POINTLIGHT_NUM];
    SpotLight spotLights[S_SPOTLIGHT_NUM];
    CircleShadow circleShadows[S_CIRCLESHADOW_NUM];
}

cbuffer ConstBufferDataMaterial2 : register(b3)
{
    float3 m_ambient : packoffset(c0); //�A���r�G���g
    float3 m_diffuse : packoffset(c1); //�f�B�t���[�Y
    float3 m_specular : packoffset(c2); //�X�y�L�����[
    float m_alpha : packoffset(c2.w); //�A���t�@
}