#include "PlaneCollider.h"


using namespace DirectX;

void PlaneCollider::Update()
{
	// ワールド行列から座標を抽出
	const M4& matWorld = object->GetMatWorld();

	// 面のメンバ変数を更新
	Plane::distance = object->worldMat->trans.GetLength();

	//法線を計算
	WorldMat worldNormal;
	worldNormal.rot = object->worldMat->rot;
	worldNormal.SetWorld();
	//回転のみ
	Vec3 normal = GetVec3xM4({ 0,0,-1.0f }, worldNormal.matWorld, 0).GetNormalized();
	Plane::normal = { normal.x,normal.y,normal.z,0 };
}
