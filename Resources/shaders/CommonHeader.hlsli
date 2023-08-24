

Texture2D<float4> tex : register(t0); //0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
Texture2D<float4> tex2 : register(t1); //1�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��(�f�B�]���u)
Texture2D<float4> tex3 : register(t2); //2�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��(�X�y�L�����}�b�v�e�N�X�`��)
Texture2D<float4> tex4 : register(t3); //3�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��(�m�[�}���}�b�v�}�b�v�e�N�X�`��)
SamplerState smp : register(s0); //0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

struct PSOutput
{
    float4 col : SV_TARGET0; //�ʏ�̃����_�����O
    float4 col2 : SV_TARGET1; //�F�Q
    float4 highLumi : SV_TARGET2; //���P�x
};

//----------------------------------------------------------------------------------------------
//�X�y�L�����}�b�v
float4 GetSpecularMapColor(float3 specular, float3 diffuse, float4 maskCol)
{
    float4 col = float4(float3(maskCol.r, maskCol.r, maskCol.r) * (specular + diffuse), 0);
    return col;
}

//�m�[�}���}�b�v
float3 GetNormalMapWorldNormalVec(float4 normalMapCol, float3 tangent,
    float3 binormal, float3 normal, float4x4 world)
{
    //�m�[�}���}�b�v�摜�̐F��@���x�N�g���ɕϊ�(-1�`1)
    float3 nMap = normalize(normalMapCol.rgb * 2.0f - 1.0f);
    //�ڐ����m�[�}���C�Y
    float3 tangentL = normalize(tangent);
    //�]�@�����m�[�}���C�Y
    float3 binormalL = normalize(binormal);
    //���_�̖@�����m�[�}���C�Y
    float3 normalL = normalize(normal);
    
    //�m�[�}���}�b�v����Z�o�����@���x�N�g���͐ڋ�Ԃɂ���̂ŁA���[�J����Ԃɕϊ�
    float3 lNormal = normalize(tangentL * nMap.r + binormalL * nMap.g + normalL * nMap.b);
    
    //���[�J�����烏�[���h��
    //�@���Ƀ��[���h�s��ɂ��X�P�[�����O�E��]��K�p
    float3 wNormal = mul((float3x3) world, lNormal);
    
    return wNormal;
}


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
//�}�e���A��
cbuffer ConstBufferDataMaterial : register(b0)
{
    float4 color; //�F(RGBA)
}

//3D�ϊ��s��
cbuffer ConstBufferDataTransform : register(b1)
{
    matrix viewproj; //�r���[�v���W�F�N�V�����s��
    matrix world; //���[���h�s��
    float3 cameraPos; //�J�������W�i���[���h���W�j
}

//LightManager��ConstBuffer�Ɠ����^��錾
cbuffer ConstBufferDataMaterial3 : register(b4)
{
    float3 ambientColor;
    float3 diffuseColor;
    float3 specularColor;
    DirLight dirLights[S_DIRLIGHT_NUM];
    PointLight pointLights[S_POINTLIGHT_NUM];
    SpotLight spotLights[S_SPOTLIGHT_NUM];
    CircleShadow circleShadows[S_CIRCLESHADOW_NUM];
}

cbuffer ConstBufferEffectFlags : register(b5)
{
	//�t�H�O
    uint isFog;
	//�g�D�[��
    uint isToon;
	//�������C�g
    uint isRimLight;
	//�����J���[
    float4 rimColor;
	//�^���V���G�b�g
    uint isSilhouette;
	//�f�B�]���u
    uint isDissolve = false;
	//�f�B�]���u����
    float dissolveT = 0;
    //�X�y�L�����}�b�v
    uint isSpecularMap = false;
    //�m�[�}���}�b�v
    uint isNormalMap = false;
	//����
    uint time;
}