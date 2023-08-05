#include"PeraShaderHeader.hlsli"

//�T���v���[
SamplerState smp : register(s0);

//�N���X�t�B���^�p�ɓ�+��ʊE�[�x�p
struct PSOutput
{
	float4 blur : SV_TARGET0;//���P�x�ڂ���
	float4 blur2 : SV_TARGET1;//���P�x�ڂ����Q
	float4 col : SV_TARGET2;//�ʏ�ڂ����i��ʊE�[�x�p�j
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
		output.blur = Gaussian2(tex2, smp, input.uv);
	}

	if (isCrossFilter)
	{
		//���P�x�ڂ����ꖇ�ڂɂ���
		output.blur = GaussianAngle(tex2, smp, 45, input.uv);
		output.blur += GaussianAngle(tex2, smp, 90, input.uv);
		//���P�x�ڂ���2���ڂɂ���
		output.blur2 = GaussianAngle(tex2, smp, -45, input.uv);
		output.blur2 += GaussianAngle(tex2, smp, -90, input.uv);
	}
	//��ʊE�[�x�p
	if(isDepthField)
    {
        output.col = Gaussian2(tex0, smp, input.uv);
    }

	return output;
}
