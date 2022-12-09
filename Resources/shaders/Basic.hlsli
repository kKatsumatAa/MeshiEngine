
//�}�e���A��
cbuffer ConstBufferDataMaterial : register(b0)
{
	float4 color;//�F(RGBA)
}

//���_�V�F�[�_�[�̏o�͍\����
//�i���_�V�F�[�_�[����s�N�Z���V�F�[�_�[�ւ̂����Ɏg�p����j
struct VSOutput
{
	//�V�X�e���p���_���W
	float4 svpos : SV_POSITION;
	//�@���x�N�g��
	//float3 normal : NORMAL;
	// 
	float4 color : COLOR;

	//uv�l
	float2 uv : TEXCOORD;
};


//3D�ϊ��s��
cbuffer ConstBufferDataTransform : register(b1)
{
	matrix viewproj; //�r���[�v���W�F�N�V�����s��
	matrix world;    //���[���h�s��
	float3 cameraPos;//�J�������W�i���[���h���W�j
}

cbuffer ConstBufferDataMaterial3 : register(b3)
{
	float3 lightv;     //���C�g�ւ̕����x�N�g��
	float3 lightcolor; //���C�g�̐F�iRGB�j
}