#include "Basic.hlsli"

struct VSOutPutS
{
    float4 pos : SV_POSITION;
};

//�V���h�E�}�b�v�p���_���W�ϊ�
VSOutPutS main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD,
float4 tangent : TANGENT, float4 binormal : BINORMAL)
{
    VSOutPutS o;
    
    o.pos = mul(mul(lightCamera, world), pos);
        
    //���C�g�J�������g��
    return o;
}