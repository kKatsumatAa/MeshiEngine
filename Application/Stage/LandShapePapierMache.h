/*
* @file LandShapePapierMache.h
* @brief 見た目だけの地形オブジェクト
*/

#pragma once
#include"Object.h"


//張りぼての地形（当たり判定はなく、ワイヤーフレーム）
class LandShapePapierMache : public Object
{
public:
	//インスタンス生成
	static std::unique_ptr<LandShapePapierMache> Create(WorldMat* worldMat, IModel* model);
	//初期化
	bool Initialize(WorldMat* worldMat, IModel* model);
	//描画
	void Draw(float vel);

	//親クラスにUpdate,OnCollision
};