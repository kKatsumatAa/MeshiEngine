
//3D変換行列
cbuffer ConstBufferDataTransform : register(b1)
{
	matrix mat;//3D変換行列
}

cbuffer ConstBufferDataMaterial2 : register(b2)
{
	float3 m_ambient  : packoffset(c0);//アンビエント
	float3 m_diffuse  : packoffset(c1);//ディフューズ
	float3 m_specular : packoffset(c2);//スペキュラー
	float  m_alpha    : packoffset(c2.w);//アルファ
}

//頂点シェーダからピクセルシェーダーへのやり取りに使用する構造体
struct VSOutput
{
	float4 svpos  : SV_POSITION;//システム用頂点座標
	float3 normal : NORMAL;//法線ベクトル
	float2 uv     : TEXCOORD;//uv座標
};