#include "Sprite.hlsli"

Texture2D<float4> tex : register(t0); //0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
Texture2D<float4> tex2 : register(t1); //1�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0);      //0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[


float4 main(VSOutput input) : SV_TARGET
{
	//�e�N�X�`��
	float4 texcolor = float4(tex.Sample(smp, input.uv));
	
	{
		return texcolor * color;
	}
}