


cbuffer ConstBufferEffectFlags : register(b0)
{
	//�ڂ���
	unsigned int isGaussian;
	//�K�E�V�A���ڂ���
	unsigned int isGaussian2;
	//�G���{�X
	unsigned int isEmboss;
	//�V���[�v�l�X
	unsigned int isSharpness;
	//�~��
	unsigned int isGradation;
	//�A�E�g���C��
	unsigned int isOutLine;
	//�r�l�b�g
	unsigned int isVignette;
	//�M��p��
	unsigned int isBarrelCurve;
	//������
	unsigned int isScanningLine;
	//�O���[�X�P�[��
	unsigned int isGrayScale;
	//�K���X�t�B���^�[
	unsigned int isGlassFilter;
	//���U�C�N
	unsigned int isMosaic;
	//�l�K�|�W���]
	unsigned int isNega;
	//�l�K�|�W���]
	unsigned int isRGBShift;
	//�u���[��
	unsigned int isBloom;
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
//�ꖇ�ڂ̎l��(�k���o�b�t�@���P�x)
Texture2D<float4> tex3 : register(t3);
//�K���X�t�B���^�[
Texture2D<float4> effectTex : register(t4);


//�ڂ�������̉摜��Ԃ�
float4 Get5x5GaussianBlur(Texture2D<float4> tex, SamplerState smp, float2 uv, float dx, float dy, float4 rect) {
	float4 ret = tex.Sample(smp, uv);

	float l1 = -dx, l2 = -2 * dx;
	float r1 = dx, r2 = 2 * dx;
	float u1 = -dy, u2 = -2 * dy;
	float d1 = dy, d2 = 2 * dy;
	l1 = max(uv.x + l1, rect.x) - uv.x;
	l2 = max(uv.x + l2, rect.x) - uv.x;
	r1 = min(uv.x + r1, rect.z - dx) - uv.x;
	r2 = min(uv.x + r2, rect.z - dx) - uv.x;

	u1 = max(uv.y + u1, rect.y) - uv.y;
	u2 = max(uv.y + u2, rect.y) - uv.y;
	d1 = min(uv.y + d1, rect.w - dy) - uv.y;
	d2 = min(uv.y + d2, rect.w - dy) - uv.y;

	return float4((
		tex.Sample(smp, uv + float2(l2, u2)).rgb
		+ tex.Sample(smp, uv + float2(l1, u2)).rgb * 4
		+ tex.Sample(smp, uv + float2(0, u2)).rgb * 6
		+ tex.Sample(smp, uv + float2(r1, u2)).rgb * 4
		+ tex.Sample(smp, uv + float2(r2, u2)).rgb

		+ tex.Sample(smp, uv + float2(l2, u1)).rgb * 4
		+ tex.Sample(smp, uv + float2(l1, u1)).rgb * 16
		+ tex.Sample(smp, uv + float2(0, u1)).rgb * 24
		+ tex.Sample(smp, uv + float2(r1, u1)).rgb * 16
		+ tex.Sample(smp, uv + float2(r2, u1)).rgb * 4

		+ tex.Sample(smp, uv + float2(l2, 0)).rgb * 6
		+ tex.Sample(smp, uv + float2(l1, 0)).rgb * 24
		+ ret.rgb * 36
		+ tex.Sample(smp, uv + float2(r1, 0)).rgb * 24
		+ tex.Sample(smp, uv + float2(r2, 0)).rgb * 6

		+ tex.Sample(smp, uv + float2(l2, d1)).rgb * 4
		+ tex.Sample(smp, uv + float2(l1, d1)).rgb * 16
		+ tex.Sample(smp, uv + float2(0, d1)).rgb * 24
		+ tex.Sample(smp, uv + float2(r1, d1)).rgb * 16
		+ tex.Sample(smp, uv + float2(r2, d1)).rgb * 4

		+ tex.Sample(smp, uv + float2(l2, d2)).rgb
		+ tex.Sample(smp, uv + float2(l1, d2)).rgb * 4
		+ tex.Sample(smp, uv + float2(0, d2)).rgb * 6
		+ tex.Sample(smp, uv + float2(r1, d2)).rgb * 4
		+ tex.Sample(smp, uv + float2(r2, d2)).rgb
		) / 256.0f, ret.a);
}
