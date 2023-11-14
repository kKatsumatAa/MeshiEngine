
#include"CommonHeader.hlsli"


float4 WavePos(float4 localPos, float3 normal, matrix world)
{
    float maxWidth = 0;
    
    for (int i = 0; i < WAVE_NUM; i++)
    {
        maxWidth += max(waves[i].waveThickness.y * max(waves[i].waveThickness.x - abs(
            waves[i].waveDistance - distance(mul(world, localPos).xyz, waves[i].waveEpicenter)), 0)
            - maxWidth
        , 0);
    }
    
    
    return float4(mul(world, localPos).xyz +
        normal * maxWidth, 1.0f);
}

struct HS_CONSTANT_DATA_OUTPUT
{
    float EdgeTessFactor[3] : SV_TessFactor;
    float InsideTessFactor : SV_InsideTessFactor;
};

#define NUM_CONTROL_POINTS 3

[domain("tri")]
DSOutput main(
	HS_CONSTANT_DATA_OUTPUT input, //�p�b�`�萔�֐�����̏o�͒l
	float3 domain : SV_DomainLocation, //�p�b�`���ł̈ʒu
	const OutputPatch<VSOutput2, NUM_CONTROL_POINTS> patch //�n���V�F�[�_�[����o�͂��ꂽ����_
)
{
    //���b�V���̍s��ƃ��[���h��������    
    float4x4 worldL = mul(world, worldMesh);
    matrix viewWorld = mul(viewproj, worldL);
    
    
    DSOutput Output;
    
    //�@��
    Output.normal = float3(
		patch[0].normal.xyz * domain.x + patch[1].normal.xyz * domain.y + patch[2].normal.xyz * domain.z);
    
    //�V�X�e���p���W
    Output.svpos = float4(
		patch[0].svpos.xyz * domain.x + patch[1].svpos.xyz * domain.y + patch[2].svpos.xyz * domain.z, 1);
    Output.svpos = WavePos(Output.svpos, Output.normal, worldL);
    Output.svpos = mul(viewproj, Output.svpos);

    //���[���h���W
    Output.worldpos = float4(
		patch[0].worldpos.xyz * domain.x + patch[1].worldpos.xyz * domain.y + patch[2].worldpos.xyz * domain.z, 1);
    Output.worldpos = WavePos(Output.worldpos, Output.normal, worldL);
    //
    Output.tpos = float4(
		patch[0].tpos.xyz * domain.x + patch[1].tpos.xyz * domain.y + patch[2].tpos.xyz * domain.z, 1);
    Output.tpos = mul(lightCamera, Output.worldpos);
    
    Output.binormal = float3(
		patch[0].binormal.xyz * domain.x + patch[1].binormal.xyz * domain.y + patch[2].binormal.xyz * domain.z);
    Output.tangent = float3(
		patch[0].tangent.xyz * domain.x + patch[1].tangent.xyz * domain.y + patch[2].tangent.xyz * domain.z);
    Output.uv = float2(
		patch[0].uv.xy * domain.x + patch[1].uv.xy * domain.y + patch[2].uv.xy * domain.z);

    return Output;
}
