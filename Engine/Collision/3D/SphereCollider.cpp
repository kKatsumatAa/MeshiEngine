#include "SphereCollider.h"


using namespace DirectX;

void SphereCollider::Update()
{
	// ワールド行列から座標を抽出
	const M4& matWorld = object_->GetMatWorld();

	// 球のメンバ変数を更新
	Sphere::center = {
		(float)matWorld.m_[3][0] + offset_.m128_f32[0],
	(float)matWorld.m_[3][1] + offset_.m128_f32[1],
	(float)matWorld.m_[3][2] + offset_.m128_f32[2],
	(float)matWorld.m_[3][3] + offset_.m128_f32[3],
	};
	//ワールド行列のスケールから半径
	Sphere::radius =
		(object_->GetScale().x + object_->GetScale().y + object_->GetScale().z) / 3.0f;
}
