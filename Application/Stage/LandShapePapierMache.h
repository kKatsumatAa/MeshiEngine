/*
* @file LandShapePapierMache.h
* @brief 見た目だけの地形オブジェクト
*/

#pragma once
#include"Object.h"


//張りぼての地形（当たり判定はなく、ワイヤーフレーム）
class LandShapePapierMache : public Object
{
private:
	const Vec3 MODEL_MATERIAL_EXTEND_ = { 0.45f, 0.4f, 5.0f };
	const Vec3 COLOR_RGB_ = { 1.0f,1.0f,1.0f };
	const float COLOR_A_MIN_ = -0.3f;
	const float COLOR_A_MAX_ = -0.3f;

public:
	//インスタンス生成
	static std::unique_ptr<LandShapePapierMache> Create(WorldMat* worldMat, IModel* model);
	//初期化
	bool Initialize(WorldMat* worldMat, IModel* model);
	//描画
	void Draw(float vel);

	//親クラスにUpdate,OnCollision
};