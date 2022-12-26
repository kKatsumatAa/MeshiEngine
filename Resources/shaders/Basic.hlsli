
//マテリアル
cbuffer ConstBufferDataMaterial : register(b0)
{
	float4 color;//色(RGBA)
}

//頂点シェーダからピクセルシェーダーへのやり取りに使用する構造体
struct VSOutput
{
	float4 svpos    : SV_POSITION;//システム用頂点座標
	float4 worldpos : POSITION;//ワールド座標
	float3 normal   : NORMAL;//ワールド座標
	float2 uv       : TEXCOORD;//uv座標
};


//3D変換行列
cbuffer ConstBufferDataTransform : register(b1)
{
	matrix viewproj; //ビュープロジェクション行列
	matrix world;    //ワールド行列
	float3 cameraPos;//カメラ座標（ワールド座標）
}

// 点光源の数
static const int POINTLIGHT_NUM = 6;
struct PointLight
{
	float3 lightpos;//ライトの座標
	float3 lightcolor;//ライトの色
	float3 lightatten;//ライトの距離減衰係数
	uint   active;//有効か
};

// 平行光源の数
static const int DIRLIGHT_NUM = 3;
struct DirLight
{
	float3 lightv;    // ライトへの方向の単位ベクトル
	float3 lightcolor;    // ライトの色(RGB)
	uint   active;      //有効か
};

//LightManagerのConstBufferと同じ型を宣言
cbuffer ConstBufferDataMaterial3 : register(b3)
{
	float3     ambientColor;
	DirLight   dirLights[DIRLIGHT_NUM];
	PointLight pointLights[POINTLIGHT_NUM];
}