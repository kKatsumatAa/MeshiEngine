#include "SphereCollider.h"


using namespace DirectX;

void SphereCollider::Update()
{
	// ���[���h�s�񂩂���W�𒊏o
	const M4& matWorld = object->GetMatWorld();

	// ���̃����o�ϐ����X�V
	Sphere::center = {
		(float)matWorld.m[3][0] + offset.m128_f32[0],
	(float)matWorld.m[3][1] + offset.m128_f32[1],
	(float)matWorld.m[3][2] + offset.m128_f32[2],
	(float)matWorld.m[3][3] + offset.m128_f32[3],
	};
	//���[���h�s��̃X�P�[�����甼�a
	Sphere::radius =
		(object->worldMat->scale.x + object->worldMat->scale.y + object->worldMat->scale.z) / 3.0f;
}
