#include "SphereCollider.h"


using namespace DirectX;

void SphereCollider::Update()
{
	// ワールド行列から座標を抽出
	const M4& matWorld = object->GetMatWorld();

	// 球のメンバ変数を更新
	Sphere::center = {
		(float)matWorld.m[3][0] + offset.m128_f32[0],
	(float)matWorld.m[3][1] + offset.m128_f32[1],
	(float)matWorld.m[3][2] + offset.m128_f32[2],
	(float)matWorld.m[3][3] + offset.m128_f32[3],
	};
	//ワールド行列のスケールから半径
	Sphere::radius =
		(object->worldMat->scale.x + object->worldMat->scale.y + object->worldMat->scale.z) / 3.0f;
}
