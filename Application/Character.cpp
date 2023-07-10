#include "Character.h"

void Character::PickUpWeapon(Weapon* weapon, Vec3* localPos)
{
	SetWeapon(weapon);

	if (localPos)
	{
		weapon_->SetLocalPos(*localPos);
	}

	weapon_->ChangeOwner(this);
	//���L�҂̔��葮��������
	weapon_->SetAttribute(GetCollider()->GetAttribute());
}

void Character::FallWeapon(const Vec3& directionVec, Vec3* localPos)
{
	if (localPos)
	{
		GetWeapon()->SetLocalPos(*localPos);
	}

	GetWeapon()->ChangeOwner(nullptr);
	GetWeapon()->SetFallVec(directionVec);
	//���Ŏ肩�痣�ꂽ��A�C�e���̑����ɂ���
	weapon_->SetAttribute(COLLISION_ATTR_ITEMS);
	SetWeapon(nullptr);
}

void Character::GroundUpdate(float LengthY, float velocityYPow, bool isJump, std::function<void()>f)
{
	//��������
	if (!isOnGround_)
	{
		//����
		fallVec_.y_ = max((fallVec_.y_ + FALL_ACC_ * velocityYPow) , FALL_V_Y_MIN_);
		//�ړ�
		SetTrans(GetTrans() + fallVec_ * velocityYPow);
		//�W�����v�̏����g���K�[�������ĂȂ�������
		if (f && !isJump)
		{
			f();
		}
	}
	//�W�����v����
	else if (isJump)
	{
		isOnGround_ = false;
		fallVec_ = { 0,JUMP_V_Y_FIST_,0 };
		//�֐��̏������s
		if (f)
		{
			f();
		}
	}

	Object::Update();

	//��[���牺�[�܂ł̃��C�L���X�g�p���C������(�����蔻��͐e�q�֌W���l��)
	Ray ray;
	ray.start = { GetWorldTrans().x_,GetWorldTrans().y_, GetWorldTrans().z_ };
	//��[
	ray.start.m128_f32[1] += LengthY;
	ray.dir = { 0,-1.0f,0 };
	RaycastHit info;

	//�ڒn���
	if (isOnGround_)
	{
		//�X���[�Y�ɍ�����邽�߂̋z������(�������߂ɂ��邱�Ƃ�)
		const float adsDistance = LengthY * 0.2f;
		//�ڒn���ێ�
		if (CollisionManager::GetInstance()->Raycast(
			ray, COLLISION_ATTR_LANDSHAPE, &info, LengthY * 2.0f + adsDistance))
		{
			isOnGround_ = true;
			//�߂荞�ݕ����
			SetTrans(GetTrans() - Vec3(0, info.distance - LengthY * 2.0f, 0));
			//
			Object::Update();
		}
		//�n�ʂ��Ȃ��̂ŗ���
		else
		{
			isOnGround_ = false;
			//������0
			fallVec_ = {};
		}
	}
	//�������
	else if (fallVec_.y_ <= 0.0f)
	{
		if (CollisionManager::GetInstance()->Raycast(
			ray, COLLISION_ATTR_LANDSHAPE, &info, LengthY * 2.0f))
		{
			//���n
			isOnGround_ = true;
			SetTrans(GetTrans() - Vec3(0, info.distance - LengthY * 2.0f, 0));
			Object::Update();
		}
	}

}
