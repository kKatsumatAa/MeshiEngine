#include "TriangleCollider.h"

using namespace DirectX;

void TriangleCollider::Update()
{
	// 面のメンバ変数を更新
	float angle = ANGLE_1_;
	float angle2 = ANGLE_2_;
	float angle3 = ANGLE_3_;

	Triangle::iP0 = { GetVec3xM4({ sinf(angle),cosf(angle),0 }, object_->GetMatWorld(), true).x,
		GetVec3xM4({ sinf(angle),cosf(angle),0 }, object_->GetMatWorld(), true).y,
		GetVec3xM4({ sinf(angle),cosf(angle),0 }, object_->GetMatWorld(), true).z
	};//左下

	Triangle::iP1 = { GetVec3xM4({ 0,cosf(angle2),0 }, object_->GetMatWorld(), true).x,
		GetVec3xM4({ 0,cosf(angle2),0 }, object_->GetMatWorld(), true).y,
		GetVec3xM4({ 0,cosf(angle2),0 }, object_->GetMatWorld(), true).z
	};//左下

	Triangle::iP2 = { GetVec3xM4({ sinf(angle3),cosf(angle3),0 }, object_->GetMatWorld(), true).x,
		GetVec3xM4({ sinf(angle3),cosf(angle3),0 }, object_->GetMatWorld(), true).y,
		GetVec3xM4({ sinf(angle3),cosf(angle3),0 }, object_->GetMatWorld(), true).z
	};//左下

	//法線を計算
	Triangle::ComputeNormal();
}

inline void TriangleCollider::SetPos(const DirectX::XMVECTOR& p0, const DirectX::XMVECTOR& p1, const DirectX::XMVECTOR& p2)
{
	p0_ = { p0.m128_f32[0],p0.m128_f32[1],p0.m128_f32[2],p0.m128_f32[3] };
	p1_ = { p1.m128_f32[0],p1.m128_f32[1],p1.m128_f32[2],p1.m128_f32[3] };
	p2_ = { p2.m128_f32[0],p2.m128_f32[1],p2.m128_f32[2],p2.m128_f32[3] };
}
