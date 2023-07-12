#include "Character.h"

using namespace DirectX;


const float Character::IS_WALL_ROT_ = 30.0f;

void Character::PickUpWeapon(Weapon* weapon, Vec3* localPos)
{
	SetWeapon(weapon);

	if (localPos)
	{
		weapon_->SetLocalPos(*localPos);
	}

	weapon_->ChangeOwner(this);
	//���L�҂̔��葮��������
	if (GetCollider())
	{
		weapon_->SetAttribute(GetCollider()->GetAttribute());
	}
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
	weapon_->SetIsThrowing(true);
	SetWeapon(nullptr);
}

void Character::OnGroundAndWallUpdate(float LengthY, float velocityYPow, bool isJump, std::function<void()>f)
{
	//��������
	if (!isOnGround_)
	{
		//����
		fallVec_.y_ = max((fallVec_.y_ + FALL_ACC_ * velocityYPow), FALL_V_Y_MIN_);
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
	//�s��X�V
	Object::WorldMatColliderUpdate();

	//�ǂƂ̓����蔻��
	QueryCallBackUpdate();

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
			//�s��X�V
			Object::WorldMatColliderUpdate();
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
			Object::WorldMatColliderUpdate();
		}
	}

}

void Character::QueryCallBackUpdate()
{
	//���R���C�_�[�擾
	SphereCollider* sphereColl = dynamic_cast<SphereCollider*>(GetCollider());

	//�N�G���[�R�[���o�b�N�N���X
	class CharacterQueryCallBack :public QueryCallback
	{
	public:
		CharacterQueryCallBack(Sphere* sphere) : sphere(sphere) {};

		// �Փˎ��R�[���o�b�N�֐�
		bool OnQueryHit(const QueryHit& info)
		{
			//�����
			const XMVECTOR up = { 0,1.0f,0,0 };
			//�r�˕���
			XMVECTOR rejectDir = XMVector3Normalize(info.reject);
			//������Ɣr�˕����̊p�x���̃R�T�C���l
			float cos = XMVector3Dot(rejectDir, up).m128_f32[0];

			// �n�ʔ��肵�����l�p�x
			const float threshold = cosf(XMConvertToRadians(IS_WALL_ROT_));
			//�p�x���ɂ���ēV��܂��͒n�ʂƔ��肳���ꍇ��������
			if (-threshold < cos && cos < threshold) {
				sphere->center += info.reject;
				move += info.reject;
			}

			return true;
		}

		Sphere* sphere = nullptr;
		DirectX::XMVECTOR move = {};
	};

	//�N�G���[�R�[���o�b�N�̊֐��I�u�W�F�N�g
	CharacterQueryCallBack callback(sphereColl);

	// ���ƒn�`�̌�����S����
	CollisionManager::GetInstance()->QuerySphere(*sphereColl, &callback, COLLISION_ATTR_LANDSHAPE);
	// �����ɂ��r�˕�������
	SetTrans(GetTrans()
		+ Vec3(callback.move.m128_f32[0], callback.move.m128_f32[1], callback.move.m128_f32[2]));
	// ���[���h�s��X�V
	Object::WorldMatColliderUpdate();
}
