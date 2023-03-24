#include"PeraShaderHeader.hlsli"

Output VS(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
	Output output;
	output.svpos = pos;
	output.normal = normal;
	output.uv = uv;

	return output;
}