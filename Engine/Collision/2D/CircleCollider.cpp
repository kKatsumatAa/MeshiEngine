#include "CircleCollider.h"


using namespace DirectX;

void CircleCollider::Update()
{
	// ワールド行列から座標を抽出
	const M4& matWorld = object_->GetMatWorld();

	// 円のメンバ変数を更新(スケールのx,yのみ)
	Circle::center = {
		(float)matWorld.m_[3][0],
	(float)matWorld.m_[3][1],
	};
	//ワールド行列のスケールから半径
	Circle::iRadius =
		(object_->GetScale().x * SCALE_RATE_ + object_->GetScale().y * SCALE_RATE_) / (float)S_MEAN_VALUE_NUM_;
}
