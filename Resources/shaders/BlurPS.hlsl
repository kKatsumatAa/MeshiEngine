#include"PeraShaderHeader.hlsli"

//�T���v���[
SamplerState smp : register(s0);

//�N���X�t�B���^�p�ɓ�
struct PSOutput
{
	float4 blur : SV_TARGET0;//���P�x�ڂ���
	float4 blur2 : SV_TARGET1;//���P�x�ڂ����Q
};

//���C���e�N�X�`�����ڂ���
PSOutput BlurPS(Output input)
{

	PSOutput output;

	float w, h, miplevels;
	tex2.GetDimensions(0, w, h, miplevels);
	float dx = 2.0f / w;
	float dy = 2.0f / h;
	float4 ret;

	if (isBloom)
	{
		output.blur = Get5x5GaussianBlur(tex2, smp, input.uv, dx, dy, float4(0, 0, 1, 1));
	}

	if (isCrossFilter)
	{
		//���P�x�ڂ����ꖇ�ڂɂ���
		output.blur = GaussianAngle(tex2, smp, 30, input.uv);
		//���P�x�ڂ����ꖇ�ڂɂ���
		output.blur2 = GaussianAngle(tex2, smp, -30, input.uv);
	}

	return output;
}
