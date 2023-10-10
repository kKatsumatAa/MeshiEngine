#include "TriangleCollider.h"

using namespace DirectX;

void TriangleCollider::Update()
{
	// 髱｢縺ｮ繝｡繝ｳ繝仙､画焚繧呈峩譁ｰ
	float angle = PI * 2.0f + PI * 2.0f / 3.0f * 2.0f;
	float angle2 = PI * 2.0f;
	float angle3 = PI * 2.0f + PI * 2.0f / 3.0f;

	Triangle::iP0 = { GetVec3xM4({ sinf(angle),cosf(angle),0 }, object_->GetMatWorld(), 1).x,
		GetVec3xM4({ sinf(angle),cosf(angle),0 }, object_->GetMatWorld(), 1).y,
		GetVec3xM4({ sinf(angle),cosf(angle),0 }, object_->GetMatWorld(), 1).z
	};//蟾ｦ荳・

	Triangle::iP1 = { GetVec3xM4({ 0,cosf(angle2),0 }, object_->GetMatWorld(), 1).x,
		GetVec3xM4({ 0,cosf(angle2),0 }, object_->GetMatWorld(), 1).y,
		GetVec3xM4({ 0,cosf(angle2),0 }, object_->GetMatWorld(), 1).z
	};//蟾ｦ荳・

	Triangle::iP2 = { GetVec3xM4({ sinf(angle3),cosf(angle3),0 }, object_->GetMatWorld(), 1).x,
		GetVec3xM4({ sinf(angle3),cosf(angle3),0 }, object_->GetMatWorld(), 1).y,
		GetVec3xM4({ sinf(angle3),cosf(angle3),0 }, object_->GetMatWorld(), 1).z
	};//蟾ｦ荳・

	//豕慕ｷ壹ｒ險育ｮ・
	Triangle::ComputeNormal();
}
