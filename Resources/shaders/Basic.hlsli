
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
static const int S_POINTLIGHT_NUM = 6;
struct PointLight
{
	float3 lightpos;//ライトの座標
	float3 lightcolor;//ライトの色
	float3 lightatten;//ライトの距離減衰係数
	uint   active;//有効か
};

// 平行光源の数
static const int S_DIRLIGHT_NUM = 3;
struct DirLight
{
	float3 lightv;    // ライトへの方向の単位ベクトル
	float3 lightcolor;    // ライトの色(RGB)
	uint   active;      //有効か
};


static const int S_SPOTLIGHT_NUM = 3;
struct SpotLight
{
	float3 lightv;    // ライトへの方向の単位ベクトル
	float3 lightpos;    // ライトの位置
	float3 lightcolor;    // ライトの色(RGB)
	float3 lightatten;    // ライトの減衰係数
	float2 lightfactoranglecos;//ライト減衰角度のコサイン
	uint   active;      //有効か
};

//丸影
static const int S_CIRCLESHADOW_NUM = 1;
struct CircleShadow
{
	float3 dir; //投影方向の逆ベクトル
	float3 casterPos; //キャスター座標
	float distanceCasterLight; //キャスターとライトの距離
	float3 atten; //距離減衰係数
	float2 factorAngleCos; //減衰角度のコサイン
	uint active;
};

//LightManagerのConstBufferと同じ型を宣言
cbuffer ConstBufferDataMaterial3 : register(b3)
{
	float3     ambientColor;
	float3     diffuseColor;
	float3     specularColor;
	DirLight   dirLights[S_DIRLIGHT_NUM];
	PointLight pointLights[S_POINTLIGHT_NUM];
	SpotLight  spotLights[S_SPOTLIGHT_NUM];
	CircleShadow circleShadows[S_CIRCLESHADOW_NUM];
}

cbuffer ConstBufferEffectFlags : register(b4)
{
	//フォグ
	uint isFog;
	//トゥーン
	uint isToon;
	//リムライト
	uint isRimLight;
	//リムカラー
	float4 rimColor;
	//疑似シルエット
    uint isSilhouette;
	//ディゾルブ
    uint isDissolve = false;
	//ディゾルブ割合
    float dissolveT = 0;
	//時間
	uint time;
}
