
#include "OBJShaderHeader.hlsli"

//スキニング後の頂点・法線が入る(uvはスキニングの影響受けないのでそのまま)
struct SkinOutput
{
    float4 pos;
    float3 normal;
    uint4 boneIndices; //ボーンの番号
    float4 boneWeights; //ボーンのスキンウェイト
    float3 tangent;
};

//スキニング計算
SkinOutput ComputeSkin(SkinOutput input)
{
	//ゼロクリア
    SkinOutput output = (SkinOutput) 0;

    uint iBone; //計算するボーン情報
    float weight; //ボーンウェイト(重み)
    matrix m; //スキニング行列

	//ボーン0
    iBone = input.boneIndices.x;
    weight = input.boneWeights.x;
    m = matSkinning[iBone];
    output.pos += weight * mul(m, input.pos);
    output.normal += weight * mul((float3x3) m, input.normal);
    output.tangent += weight * mul((float3x3) m, input.tangent);

	//ボーン1
    iBone = input.boneIndices.y;
    weight = input.boneWeights.y;
    m = matSkinning[iBone];
    output.pos += weight * mul(m, input.pos);
    output.normal += weight * mul((float3x3) m, input.normal);
    output.tangent += weight * mul((float3x3) m, input.tangent);

	//ボーン2
    iBone = input.boneIndices.z;
    weight = input.boneWeights.z;
    m = matSkinning[iBone];
    output.pos += weight * mul(m, input.pos);
    output.normal += weight * mul((float3x3) m, input.normal);
    output.tangent += weight * mul((float3x3) m, input.tangent);
    
	//ボーン3
    iBone = input.boneIndices.w;
    weight = input.boneWeights.w;
    m = matSkinning[iBone];
    output.pos += weight * mul(m, input.pos);
    output.normal += weight * mul((float3x3) m, input.normal);
    output.tangent += weight * mul((float3x3) m, input.tangent);

    return output;
}


VSOutput main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD, uint4 boneIndices : BONEINDICES,
	float4 boneWeights : BONEWEIGHTS, float4 tangent : TANGENT, float4 binormal : BINORMAL)
{
    SkinOutput input = { pos, normal, boneIndices, boneWeights, tangent.rgb };
	//スキニング計算
    SkinOutput skinned = ComputeSkin(input);

    //メッシュの行列とワールドをかける
    float4 wpos = mul(worldMesh, skinned.pos);
    wpos = mul(world, wpos);
    
    float4x4 worldL = mul(world, worldMesh);
    //ワールド行列によるスケーリング・回転を適用

	//ピクセルシェーダに渡す値(ラスタライザで各ピクセルごとのワールド法線、座標が計算される)
    VSOutput output; //ピクセルシェーダに渡す値
    output.svpos = mul(mul(viewproj, worldL), skinned.pos);
    output.worldpos = wpos;
    //ローカルの法線を送り、ピクセルシェーダでワールド変換
    output.normal = normalize(skinned.normal);
    output.tangent = normalize(skinned.tangent).rgb;
    output.binormal = normalize(cross(normalize(skinned.normal), normalize(skinned.tangent.rgb)));
    output.uv = uv;

    return output;
}