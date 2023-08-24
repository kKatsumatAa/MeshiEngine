

Texture2D<float4> tex : register(t0); //0番スロットに設定されたテクスチャ
Texture2D<float4> tex2 : register(t1); //1番スロットに設定されたテクスチャ(ディゾルブ)
Texture2D<float4> tex3 : register(t2); //2番スロットに設定されたテクスチャ(スペキュラマップテクスチャ)
Texture2D<float4> tex4 : register(t3); //3番スロットに設定されたテクスチャ(ノーマルマップマップテクスチャ)
SamplerState smp : register(s0); //0番スロットに設定されたサンプラー

struct PSOutput
{
    float4 col : SV_TARGET0; //通常のレンダリング
    float4 col2 : SV_TARGET1; //色２
    float4 highLumi : SV_TARGET2; //高輝度
};

//----------------------------------------------------------------------------------------------
//スペキュラマップ
float4 GetSpecularMapColor(float3 specular, float3 diffuse, float4 maskCol)
{
    float4 col = float4(float3(maskCol.r, maskCol.r, maskCol.r) * (specular + diffuse), 0);
    return col;
}

//ノーマルマップ
float3 GetNormalMapWorldNormalVec(float4 normalMapCol, float3 tangent,
    float3 binormal, float3 normal, float4x4 world)
{
    //ノーマルマップ画像の色を法線ベクトルに変換(-1～1)
    float3 nMap = normalize(normalMapCol.rgb * 2.0f - 1.0f);
    //接線をノーマライズ
    float3 tangentL = normalize(tangent);
    //従法線もノーマライズ
    float3 binormalL = normalize(binormal);
    //頂点の法線もノーマライズ
    float3 normalL = normalize(normal);
    
    //ノーマルマップから算出した法線ベクトルは接空間にあるので、ローカル空間に変換
    float3 lNormal = normalize(tangentL * nMap.r + binormalL * nMap.g + normalL * nMap.b);
    
    //ローカルからワールドへ
    //法線にワールド行列によるスケーリング・回転を適用
    float3 wNormal = mul((float3x3) world, lNormal);
    
    return wNormal;
}


//----------------------------------------------------------------------------------------------

// 点光源の数
static const int S_POINTLIGHT_NUM = 12;
struct PointLight
{
    float3 lightpos; //ライトの座標
    float3 lightcolor; //ライトの色
    float3 lightatten; //ライトの距離減衰係数
    uint active; //有効か
};

// 平行光源の数
static const int S_DIRLIGHT_NUM = 3;
struct DirLight
{
    float3 lightv; // ライトへの方向の単位ベクトル
    float3 lightcolor; // ライトの色(RGB)
    uint active; //有効か
};


static const int S_SPOTLIGHT_NUM = 3;
struct SpotLight
{
    float3 lightv; // ライトへの方向の単位ベクトル
    float3 lightpos; // ライトの位置
    float3 lightcolor; // ライトの色(RGB)
    float3 lightatten; // ライトの減衰係数
    float2 lightfactoranglecos; //ライト減衰角度のコサイン
    uint active; //有効か
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

//--------------------
//マテリアル
cbuffer ConstBufferDataMaterial : register(b0)
{
    float4 color; //色(RGBA)
}

//3D変換行列
cbuffer ConstBufferDataTransform : register(b1)
{
    matrix viewproj; //ビュープロジェクション行列
    matrix world; //ワールド行列
    float3 cameraPos; //カメラ座標（ワールド座標）
}

//LightManagerのConstBufferと同じ型を宣言
cbuffer ConstBufferDataMaterial3 : register(b4)
{
    float3 ambientColor;
    float3 diffuseColor;
    float3 specularColor;
    DirLight dirLights[S_DIRLIGHT_NUM];
    PointLight pointLights[S_POINTLIGHT_NUM];
    SpotLight spotLights[S_SPOTLIGHT_NUM];
    CircleShadow circleShadows[S_CIRCLESHADOW_NUM];
}

cbuffer ConstBufferEffectFlags : register(b5)
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
    //スペキュラマップ
    uint isSpecularMap = false;
    //ノーマルマップ
    uint isNormalMap = false;
	//時間
    uint time;
}