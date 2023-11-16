
#include"CommonHeader.hlsli"

//�p�b�`�萔�֐��̏o�͒l
struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[3]			: SV_TessFactor;//�O�p�`�̕ӂ̕�����
	float InsideTessFactor			: SV_InsideTessFactor;//�O�p�`�����̕�����
};

#define NUM_CONTROL_POINTS 3

HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(
	InputPatch<VSOutput2, NUM_CONTROL_POINTS> ip,
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
VSOutput2 main(
	InputPatch<VSOutput2, NUM_CONTROL_POINTS> ip,
	uint i : SV_OutputControlPointID,
	uint PatchID : SV_PrimitiveID )
{
    VSOutput2 Output;

	Output.svpos = ip[i].svpos;
	Output.tpos = ip[i].tpos;
	Output.worldpos = ip[i].worldpos;
	Output.binormal = ip[i].binormal;
	Output.normal = ip[i].normal;
	Output.tangent = ip[i].tangent;
	Output.uv = ip[i].uv;

	return Output;
}
