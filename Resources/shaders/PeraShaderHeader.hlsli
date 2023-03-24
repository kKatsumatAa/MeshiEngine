


cbuffer ConstBufferEffectFlags : register(b0)
{
	//�t�H�O
	uint isFog;
	//�ڂ���
	uint isGaussian;
	//�K�E�V�A���ڂ���
	uint isGaussian2;
	//�G���{�X
	uint isEmboss;
	//�V���[�v�l�X
	uint isSharpness;
	//�~��
	uint isGradation;
	//�A�E�g���C��
	uint isOutLine;
	//�r�l�b�g
	uint isVignette;
	//�M��p��
	uint isBarrelCurve;
	//������
	uint isScanningLine;
	//�O���[�X�P�[��
	uint isGrayScale;
	//�K���X�t�B���^�[
	uint isGlassFilter;
	//����
	uint time;
}

cbuffer PostEffect : register(b1)
{
	float4 bkweights[2];
}

//vs����ps�ɑ���
struct Output
{
	float4 svpos : SV_POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
};


//�u���[���p
struct PixelOutput//(sv_target��8�܂Ŏw��ł���)
{
	float4 col : SV_TARGET0;    //�J���[�l���o��
	float4 normal : SV_TARGET1; //�@�����o��
};