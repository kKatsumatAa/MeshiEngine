#include "CircleCollider.h"


using namespace DirectX;

void CircleCollider::Update()
{
	// ワールド行列から座標を抽出
	const M4& matWorld = object_->GetMatWorld();

	// 円のメンバ変数を更新
	Circle::center = {
		(float)matWorld.m_[3][0],
	(float)matWorld.m_[3][1],
	};
	//ワールド行列のスケールから半径
	Circle::radius =
		(object_->GetScale().x / 2.0f + object_->GetScale().y / 2.0f) / 2.0f;
}
