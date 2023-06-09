


cbuffer ConstBufferEffectFlags : register(b0)
{
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
	//���U�C�N
	uint isMosaic;
	//�l�K�|�W���]
	uint isNega;
	//�l�K�|�W���]
	uint isRGBShift;
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
	float2 uv : TEXCOORD;
};


//�u���[���p
struct PixelOutput//(sv_target��8�܂Ŏw��ł���)
{
	float4 col : SV_TARGET0;    //�J���[�l���o��
	float4 normal : SV_TARGET1; //�@�����o��
};


//�ꖇ�ڂ̈��
Texture2D<float4> tex0 : register(t0);
//�ꖇ�ڂ̓��
Texture2D<float4> tex1 : register(t1);
//�K���X�t�B���^�[
Texture2D<float4> effectTex : register(t2);
//�@��(?)
Texture2D<float4> texNormal : register(t3);
//�[�x�i�H�j
Texture2D<float4> distTex : register(t4);
