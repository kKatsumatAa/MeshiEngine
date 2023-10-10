#include "PlaneCollider.h"


using namespace DirectX;

void PlaneCollider::Update()
{
	// 髱｢縺ｮ繝｡繝ｳ繝仙､画焚繧呈峩譁ｰ
	Plane::iDistance = object_->GetTrans().GetLength();

	//豕慕ｷ壹ｒ險育ｮ・
	WorldMat worldNormal;
	worldNormal.rot_ = object_->GetRot();
	worldNormal.CalcWorldMat();
	//蝗櫁ｻ｢縺ｮ縺ｿ
	Vec3 normal = GetVec3xM4({ 0,0,-1.0f }, worldNormal.matWorld_, 1);
	normal.Normalized();
	Plane::iNormal = { normal.x,normal.y,normal.z,1.0f };
}
