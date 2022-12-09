
//マテリアル
cbuffer ConstBufferDataMaterial : register(b0)
{
	float4 color;//色(RGBA)
}

//頂点シェーダーの出力構造体
//（頂点シェーダーからピクセルシェーダーへのやり取りに使用する）
struct VSOutput
{
	//システム用頂点座標
	float4 svpos : SV_POSITION;
	//法線ベクトル
	//float3 normal : NORMAL;
	// 
	float4 color : COLOR;

	//uv値
	float2 uv : TEXCOORD;
};


//3D変換行列
cbuffer ConstBufferDataTransform : register(b1)
{
	matrix viewproj; //ビュープロジェクション行列
	matrix world;    //ワールド行列
	float3 cameraPos;//カメラ座標（ワールド座標）
}

cbuffer ConstBufferDataMaterial3 : register(b3)
{
	float3 lightv;     //ライトへの方向ベクトル
	float3 lightcolor; //ライトの色（RGB）
}