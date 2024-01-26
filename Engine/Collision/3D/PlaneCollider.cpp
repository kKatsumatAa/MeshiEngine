#include "PlaneCollider.h"

using namespace Util;
using namespace DirectX;


void PlaneCollider::Update()
{
	// 面のメンバ変数を更新
	Plane::iDistance = object_->GetTrans().GetLength();

	//法線を計算
	WorldMat worldNormal;
	worldNormal.rot_ = object_->GetRot();
	worldNormal.CalcWorldMat();
	//回転のみ
	Vec3 normal = GetVec3xM4(ROT_VEC_TMP_, worldNormal.matWorld_, true);
	normal.Normalized();
	Plane::iNormal = { normal.x,normal.y,normal.z,1.0f };
}
