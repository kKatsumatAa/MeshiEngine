#include "RectangleCollider.h"

void RectangleCollider::Update()
{
	// ���[���h�s�񂩂���W�𒊏o
	const M4& matWorld = object_->GetMatWorld();

	// ��`�̃����o�ϐ����X�V
	Vec2 center = {
		(float)matWorld.m_[3][0],
	(float)matWorld.m_[3][1],
	};

	//��]�p�̍s��
	WorldMat rotWM;
	rotWM.rot_.z = object_->GetRot().z;
	rotWM.CalcRotMat();
	//�x�N�g������]�����Ē��_�Ƃ���
	Vec2 scale = { object_->GetScale().x,object_->GetScale().y };

	Vec3 leftUp = GetVec3xM4({ -scale.x,-scale.y,0 }, rotWM.GetRotMat(), 0);
	Vec3 leftDown = GetVec3xM4({ -scale.x,scale.y,0 }, rotWM.GetRotMat(), 0);
	Vec3 rightUp = GetVec3xM4({ scale.x,-scale.y,0 }, rotWM.GetRotMat(), 0);
	Vec3 rightDown = GetVec3xM4({ scale.x,scale.y,0 }, rotWM.GetRotMat(), 0);

	//���S���W�ɑ�����
	MyRectangle::leftUp = center + Vec2(leftUp.x, leftUp.y);
	MyRectangle::leftDown = center + Vec2(leftDown.x, leftDown.y);
	MyRectangle::rightUp = center + Vec2(rightUp.x, rightUp.y);
	MyRectangle::rightDown = center + Vec2(rightDown.x, rightDown.y);
};


