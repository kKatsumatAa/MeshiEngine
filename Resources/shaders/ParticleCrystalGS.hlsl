#include "ParticleLight.hlsli"

// ���_��
static const uint S_V_NUM = 12;

static const float3 vertexs[S_V_NUM] =
{
    float3(0.0, 0.5, 0.0),
float3(0.235703, -0.166665, 0.408249),
float3(0.235703, -0.166665, -0.408249),
float3(0.0, 0.5, 0.0),
float3(0.235703, -0.166665, -0.408249),
float3(-0.471405, -0.166665, 0.0),
float3(0.0, 0.5, 0.0),
float3(-0.471405, -0.166665, 0.0),
float3(0.235703, -0.166665, 0.408249),
float3(-0.471405, -0.166665, 0.0),
float3(0.235703, -0.166665, -0.408249),
float3(0.235703, -0.166665, 0.408249)
};

static const float3 normals[4] =
{
    float3(0.942809, 0.333335, -0.0),
float3(-0.471404, 0.333335, -0.816496),
float3(-0.471404, 0.333335, 0.816496),
float3(0.0, -1.0, 0.0)
};

[maxvertexcount(S_V_NUM)]
void main(
	point VSOutput input[1] : SV_POSITION,
	inout TriangleStream<GSOutput> output)
{
    GSOutput o;

    //�I�t�Z�b�g
    float4 offset = { 0, 0, 0, 0 };
    
    [unroll]
    for (int j = 0; j < 4; j++)
    {
        [unroll]
        for (int k = 0; k < 3; k++)
        {
            const int vid = j * 3 + k;
            offset = float4(vertexs[vid], 1.0f);
            //�^���I�ɉ�]
            //���S���N�_�ɉ�]������
            o.svpos = input[0].pos +
                float4(rotate(offset.xyz, (input[0].rot.x + input[0].rot.y + input[0].rot.z) / 3.0f, input[0].rot.xyz), 1.0f) * input[0].scale;
            //���[���h�ʒu
            o.worldpos = o.svpos;
            // �r���[�v���W�F�N�V�����ϊ�
            o.svpos = mul(mat, o.svpos);

            //�ʂ̒��ňႢ���o�����߂ɏ��������_���ɖ@�����Ă���������
            float3 normalL = normals[j];
            normalL = float4(rotate(normalL.xyz, (input[0].rot.x + input[0].rot.y + input[0].rot.z) / 3.0f, input[0].rot.xyz), 1.0f) +
             normalize(o.svpos - input[0].pos);
            o.normal = normalize(normalL);
            
            //�F
            o.color = input[0].color;
            
            //uv
            o.uv = float2(0, 0);
            
            output.Append(o);
        }
        output.RestartStrip();
    }
}