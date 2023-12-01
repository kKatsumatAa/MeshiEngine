
#include"CommonHeader.hlsli"


float4 WavePos(float4 localPos, float3 normal, matrix world)
{
    float maxWidth = 0;
    
    //���[���h�s����ʒu�Ɩ@���ɂ�����
    float4 wPos = mul(world, localPos);
    float4 wNormal = mul(world, float4(normal, 0));
    
    for (int i = 0; i < WAVE_NUM; i++)
    {
        //���_�̔g�̒��S�_����̋���
        float length = distance(wPos.xyz, waves[i].waveEpicenter);
        //�g�̍����̊���
        float t = max(1.0 - abs(waves[i].waveDistance - length) / waves[i].waveThickness.x, 0);
        
        maxWidth += max(waves[i].waveThickness.y * t, 0);
    }
    
    
    return float4(wPos.xyz + normalize(wNormal.xyz) * float3(maxWidth, maxWidth, maxWidth), 1.0f);
}

struct HS_CONSTANT_DATA_OUTPUT
{
    float EdgeTessFactor[3] : SV_TessFactor;
    float InsideTessFactor : SV_InsideTessFactor;
};

#define NUM_CONTROL_POINTS 3

struct DSOutPutShadow
{
    float4 pos : SV_POSITION;
};

struct HSOutPutShadow
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
};

[domain("tri")]
DSOutPutShadow main(
	HS_CONSTANT_DATA_OUTPUT input, //�p�b�`�萔�֐�����̏o�͒l
	float3 domain : SV_DomainLocation, //�p�b�`���ł̈ʒu
	const OutputPatch<HSOutPutShadow, NUM_CONTROL_POINTS> patch //�n���V�F�[�_�[����o�͂��ꂽ����_
)
{
    //���b�V���̍s��ƃ��[���h��������    
    float4x4 worldL = mul(world, worldMesh);    
    
    DSOutPutShadow Output;
    
    //�@��
    float3 normal = normalize(float3(
		patch[0].normal.xyz * domain.x + patch[1].normal.xyz * domain.y + patch[2].normal.xyz * domain.z));
    
    //�V�X�e���p���W
    Output.pos = float4(
		patch[0].pos.xyz * domain.x + patch[1].pos.xyz * domain.y + patch[2].pos.xyz * domain.z, 1);
    Output.pos = WavePos(Output.pos, normal, worldL);
    Output.pos = mul(lightCamera, Output.pos);

    return Output;
}
