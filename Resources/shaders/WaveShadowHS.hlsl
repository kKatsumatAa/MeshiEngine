
#include"CommonHeader.hlsli"

//�p�b�`�萔�֐��̏o�͒l
struct HS_CONSTANT_DATA_OUTPUT
{
    float EdgeTessFactor[3] : SV_TessFactor; //�O�p�`�̕ӂ̕�����
    float InsideTessFactor : SV_InsideTessFactor; //�O�p�`�����̕�����
};

#define NUM_CONTROL_POINTS 3

struct HSOutPutShadow
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
};

HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(
	InputPatch<HSOutPutShadow, NUM_CONTROL_POINTS> ip,
	uint PatchID : SV_PrimitiveID)
{
    HS_CONSTANT_DATA_OUTPUT Output;

    Output.EdgeTessFactor[0] =
		Output.EdgeTessFactor[1] =
		Output.EdgeTessFactor[2] =
		Output.InsideTessFactor = tessFactor;

    return Output;
}

[domain("tri")] // �e�b�Z���[�g���郁�b�V���̌`����w�肷��
[partitioning("fractional_odd")] // �p�b�`�̕����Ɏg�p����A���S���Y�����w�肷��
[outputtopology("triangle_cw")] // ���b�V���̏o�͕��@���w�肷��
[outputcontrolpoints(3)] // �n���V�F�[�_�[�̃R���g���[�� �|�C���g �t�F�[�Y���R�[��������
[patchconstantfunc("CalcHSPatchConstants")] // �Ή�����n���V�F�[�_�[�̃p�b�`�萔�t�F�[�Y�̃��C���֐�
HSOutPutShadow main(
	InputPatch<HSOutPutShadow, NUM_CONTROL_POINTS> ip,
	uint i : SV_OutputControlPointID,
	uint PatchID : SV_PrimitiveID)
{
    HSOutPutShadow Output;

    Output.pos = ip[i].pos;
    Output.normal = ip[i].normal;

    return Output;
}
