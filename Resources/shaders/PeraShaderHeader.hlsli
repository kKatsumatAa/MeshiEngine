


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

//�ꖇ�ڂ̈��(�ʏ�J���[)
Texture2D<float4> tex0 : register(t0);
//�ꖇ�ڂ̓��(�Ⴄ�J���[�i�@�����܂��j)
Texture2D<float4> tex1 : register(t1);
//�ꖇ�ڂ̎O��(���P�x�p)
Texture2D<float4> tex2 : register(t2);
//�K���X�t�B���^�[
Texture2D<float4> effectTex : register(t3);
