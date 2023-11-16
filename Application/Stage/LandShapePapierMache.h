#pragma once
#include"Object.h"


//張りぼての地形（当たり判定はなく、ワイヤーフレーム）
class LandShapePapierMache : public Object
{
public:
	static std::unique_ptr<LandShapePapierMache> Create(WorldMat* worldMat, IModel* model);

	bool Initialize(WorldMat* worldMat, IModel* model);

	void Draw(float vel);

	//親クラスにUpdate,OnCollision
};