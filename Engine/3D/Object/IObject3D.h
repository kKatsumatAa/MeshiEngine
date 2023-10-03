#pragma once
#include"IObject.h"
#include "LightManager.h"
#include "Camera.h"


//演出用のフラグ
struct EffectOConstBuffer
{
	//フォグ
	uint32_t isFog = false;
	//トゥーン
	uint32_t isToon = false;
	//リムライト
	uint32_t isRimLight = false;
	float pad1;
	//リムの色
	DirectX::XMFLOAT4 rimColor = { 1.0f,1.0f,1.0f,0 };
	//疑似シルエット
	uint32_t isSilhouette = false;
	//ディゾルブ
	uint32_t isDissolve = false;
	//ディゾルブ割合
	float dissolveT = 0;
	//スペキュラマップ
	uint32_t isSpecularMap = false;
	//ノーマルマップ
	uint32_t isNormalMap = false;
	//時間
	uint32_t time = 0;
};

//3Dオブジェクトの親クラス--------------------------------------------
class IObject3D : public IObject
{
private:
	ConstBuffTransform cbt_;//ここをどうにかすれば、インスタンス一つでも色々描画

private:
	//演出用バッファ
	ComPtr <ID3D12Resource> effectFlagsBuff_;
	EffectOConstBuffer* mapEffectFlagsBuff_;

};