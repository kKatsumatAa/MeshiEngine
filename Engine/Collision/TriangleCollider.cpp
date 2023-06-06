#include "TriangleCollider.h"

using namespace DirectX;

void TriangleCollider::Update()
{
	// 面のメンバ変数を更新
	float angle = pi * 2.0f + pi * 2.0f / 3.0f * 2.0f;
	float angle2 = pi * 2.0f;
	float angle3 = pi * 2.0f + pi * 2.0f / 3.0f;

	Triangle::p0 = { GetVec3xM4({ sinf(angle),cosf(angle),0 }, object->worldMat->matWorld, 1).x,
		GetVec3xM4({ sinf(angle),cosf(angle),0 }, object->worldMat->matWorld, 1).y,
		GetVec3xM4({ sinf(angle),cosf(angle),0 }, object->worldMat->matWorld, 1).z 
	};//左下

	Triangle::p1 = { GetVec3xM4({ 0,cosf(angle2),0 }, object->worldMat->matWorld, 1).x,
		GetVec3xM4({ 0,cosf(angle2),0 }, object->worldMat->matWorld, 1).y,
		GetVec3xM4({ 0,cosf(angle2),0 }, object->worldMat->matWorld, 1).z
	};//左下

	Triangle::p2 = { GetVec3xM4({ sinf(angle3),cosf(angle3),0 }, object->worldMat->matWorld, 1).x,
		GetVec3xM4({ sinf(angle3),cosf(angle3),0 }, object->worldMat->matWorld, 1).y,
		GetVec3xM4({ sinf(angle3),cosf(angle3),0 }, object->worldMat->matWorld, 1).z
	};//左下

	//法線を計算
	Triangle::ComputeNormal();
}
