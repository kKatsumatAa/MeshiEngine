#include "SphereCollider.h"


using namespace DirectX;

void SphereCollider::Update()
{
	// ���[���h�s�񂩂���W�𒊏o
	const M4& matWorld = object_->GetMatWorld();

	// ���̃����o�ϐ����X�V
	Sphere::center = {
		(float)matWorld.m_[3][0] + offset_.m128_f32[0],
	(float)matWorld.m_[3][1] + offset_.m128_f32[1],
	(float)matWorld.m_[3][2] + offset_.m128_f32[2],
	(float)matWorld.m_[3][3] + offset_.m128_f32[3],
	};
	//���[���h�s��̃X�P�[�����甼�a
	Sphere::radius =
		(object_->GetScale().x + object_->GetScale().y + object_->GetScale().z) / 3.0f;
}
