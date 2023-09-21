#include "PlaneCollider.h"


using namespace DirectX;

void PlaneCollider::Update()
{
	// 面のメンバ変数を更新
	Plane::distance = object_->GetTrans().GetLength();

	//法線を計算
	WorldMat worldNormal;
	worldNormal.rot_ = object_->GetRot();
	worldNormal.CalcWorldMat();
	//回転のみ
	Vec3 normal = GetVec3xM4({ 0,0,-1.0f }, worldNormal.matWorld_, 1);
	normal.Normalized();
	Plane::normal = { normal.x,normal.y,normal.z,1.0f };
}
