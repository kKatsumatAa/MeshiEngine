#include "Sprite.hlsli"

float4 main(VSOutput input) : SV_TARGET
{
	//�e�N�X�`��
	float4 texcolor = float4(tex.Sample(smp, input.uv));
	
	{
		return texcolor * color;
	}
}