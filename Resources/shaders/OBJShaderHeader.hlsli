
//3D変換行列
cbuffer ConstBufferDataTransform : register(b1)
{
	matrix viewproj; //ビュープロジェクション行列
	matrix world;    //ワールド行列
	float3 cameraPos;//カメラ座標（ワールド座標）
}

cbuffer ConstBufferDataMaterial2 : register(b2)
{
	float3 m_ambient  : packoffset(c0);//アンビエント
	float3 m_diffuse  : packoffset(c1);//ディフューズ
	float3 m_specular : packoffset(c2);//スペキュラー
	float  m_alpha    : packoffset(c2.w);//アルファ
}

cbuffer ConstBufferDataMaterial3 : register(b3)
{
	float3 lightv;     //ライトへの方向ベクトル
	float3 lightcolor; //ライトの色（RGB）
}

//頂点シェーダからピクセルシェーダーへのやり取りに使用する構造体
struct VSOutput
{
	float4 svpos    : SV_POSITION;//システム用頂点座標
	float4 worldpos : POSITION;//ワールド座標
	float3 normal   : NORMAL;//ワールド座標
	float2 uv       : TEXCOORD;//uv座標
};