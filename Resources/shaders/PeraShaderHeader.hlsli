


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