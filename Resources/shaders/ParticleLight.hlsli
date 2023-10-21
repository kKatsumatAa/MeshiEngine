
#include"ParticleCommon.hlsli"

// 頂点シェーダーからジオメトリシェーダーへのやり取りに使用する構造体
struct VSOutput
{
    float4 pos : POSITION; // 頂点座標
    float scale : TEXCOORD; // スケール
    float4 color : COLOR; // 色
    float3 rot : ROT; // 角度
};

//ジオメトリシェーダーからピクセルシェーダーへの出力
struct GSOutput
{
    float4 svpos : SV_POSITION; // システム用頂点座標
    float4 worldpos : POSITION; // ワールド頂点座標
    float2 uv : TEXCOORD; // uv値
    float4 color : COLOR; // 色
    float3 normal : NORMAL;//法線
};

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
cbuffer cbuff0 : register(b0)
{
    matrix mat; // ビュープロジェクション行列
    matrix matBillboard; // ビルボード行列
};

//カメラ位置
cbuffer ConstBufferDataTransform : register(b1)
{
    float3 cameraPos; //カメラ座標（ワールド座標）
}

//LightManagerのConstBufferと同じ型を宣言
cbuffer ConstBufferDataMaterial3 : register(b2)
{
    float3 ambientColor;
    float3 diffuseColor;
    float3 specularColor;
    DirLight dirLights[S_DIRLIGHT_NUM];
    PointLight pointLights[S_POINTLIGHT_NUM];
    SpotLight spotLights[S_SPOTLIGHT_NUM];
    CircleShadow circleShadows[S_CIRCLESHADOW_NUM];
}

cbuffer ConstBufferDataMaterial2 : register(b3)
{
    float3 m_ambient : packoffset(c0); //アンビエント
    float3 m_diffuse : packoffset(c1); //ディフューズ
    float3 m_specular : packoffset(c2); //スペキュラー
    float m_alpha : packoffset(c2.w); //アルファ
}