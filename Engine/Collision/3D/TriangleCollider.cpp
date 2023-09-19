#include "TriangleCollider.h"

using namespace DirectX;

void TriangleCollider::Update()
{
	// �ʂ̃����o�ϐ����X�V
	float angle = PI * 2.0f + PI * 2.0f / 3.0f * 2.0f;
	float angle2 = PI * 2.0f;
	float angle3 = PI * 2.0f + PI * 2.0f / 3.0f;

	Triangle::p0 = { GetVec3xM4({ sinf(angle),cosf(angle),0 }, object_->GetMatWorld(), 1).x,
		GetVec3xM4({ sinf(angle),cosf(angle),0 }, object_->GetMatWorld(), 1).y,
		GetVec3xM4({ sinf(angle),cosf(angle),0 }, object_->GetMatWorld(), 1).z 
	};//����

	Triangle::p1 = { GetVec3xM4({ 0,cosf(angle2),0 }, object_->GetMatWorld(), 1).x,
		GetVec3xM4({ 0,cosf(angle2),0 }, object_->GetMatWorld(), 1).y,
		GetVec3xM4({ 0,cosf(angle2),0 }, object_->GetMatWorld(), 1).z
	};//����

	Triangle::p2 = { GetVec3xM4({ sinf(angle3),cosf(angle3),0 }, object_->GetMatWorld(), 1).x,
		GetVec3xM4({ sinf(angle3),cosf(angle3),0 }, object_->GetMatWorld(), 1).y,
		GetVec3xM4({ sinf(angle3),cosf(angle3),0 }, object_->GetMatWorld(), 1).z
	};//����

	//�@�����v�Z
	Triangle::ComputeNormal();
}
