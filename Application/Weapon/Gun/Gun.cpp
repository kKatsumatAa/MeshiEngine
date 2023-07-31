#include "Gun.h"
#include "KeyboardInput.h"
#include "DebugText.h"
#include "CameraManager.h"
#include "ParticleManager.h"
#include "PadInput.h"
#include "MouseInput.h"
#include "GameVelocityManager.h"
#include "BulletManager.h"
#include "Character.h"

using namespace DirectX;


std::unique_ptr<Gun> Gun::Create(std::unique_ptr<WorldMat> worldMat)
{
	std::unique_ptr<Gun> instance = std::make_unique<Gun>();
	if (instance.get() == nullptr)
	{
		return nullptr;
	}

	//������
	if (!instance->Initialize(std::move(worldMat)))
	{
		assert(0);
	}

	return std::move(instance);
}

bool Gun::Initialize(std::unique_ptr<WorldMat> worldMat)
{
	if (!Object::Initialize(std::move(worldMat)))
	{
		return false;
	}

	//���[�J�����W
	localPos_ = GetTrans();

	return true;
}

void Gun::Attack(const Vec3& directionVec, int32_t decreBullet, Object* owner)
{
	//�N�[���^�C������Ȃ��A�c�e�Ȃ������甲����
	if (shotCoolTime_ > 0 || remainingBullets_ <= 0)
	{
		return;
	}

	//���ˍ��W(�e�{�̂ɓ�����Ȃ��悤�ɂ���)
	shotPos_ = { GetWorldTrans().x_ + directionVec.GetNormalized().x_ * GetScale().x_ * 1.1f,
		GetWorldTrans().y_ + GetScale().y_,
		GetWorldTrans().z_ + directionVec.GetNormalized().z_ * GetScale().z_ * 1.1f };

	//�e������
	BulletManager::GetInstance().CreateBullet(shotPos_, directionVec.GetNormalized() * BULLET_VELOCITY_, GetScale().x_ * 0.4f, 300, owner);

	//�p�[�e�B�N��
	ParticleGenerate({ 4.0f,4.0f,4.0f,1.5f }, { 4.0f,4.0f,4.0f,0 });

	shotCoolTime_ = SHOT_COOL_TIME_MAX_;
	remainingBullets_ -= decreBullet;
}

void Gun::ChangeOwner(Object* parent)
{
	SetRot({ 0,0,0 });
	remainingBullets_ = BULLETS_TMP_;
	shotCoolTime_ = 0;

	Weapon::ChangeOwner(parent);
}

void Gun::Update()
{
	//�e���S���Ȃ�����̓���
	NoParentMove();
	//�n�ʂ�艺�ɍs���Ȃ��悤��
	if (GetParent() == nullptr)
	{
		SetTransY(max(GetTrans().y_, GetScale().x_));
	}

	//�N�[���^�C�����Q�[���X�s�[�h��������
	shotCoolTime_ -= 1.0f * GameVelocityManager::GetInstance().GetVelocity();

	Object::Update();
}


//----------------------------------------------------------------------------------------------------------------
void Gun::ParticleGenerate(const XMFLOAT4& sColor, const XMFLOAT4& eColor)
{
	//�p�[�e�B�N��
	for (int32_t i = 0; i < 30; ++i)
	{
		const float MD_VEL = 0.6f;
		Vec3 vel{};
		vel.x_ = GetRand(-0.5f, 0.5f);
		vel.y_ = GetRand(-0.5f, 0.5f);
		vel.z_ = GetRand(-0.5f, 0.5f);

		float scale = GetRand(GetScale().x_ / 2.0f, GetScale().x_ * 2.0f);

		ParticleManager::GetInstance()->Add(40, shotPos_, vel, { 0,0,0 }, scale, 0, sColor, eColor);
	}

}

void Gun::OnLandShape(const Vec3& interPos)
{
	SetIsAlive(false);

	//�p�[�e�B�N��
	for (int32_t i = 0; i < 20; ++i)
	{
		Vec3 vel{};
		vel.x_ = GetRand(-0.2f, 0.2f);
		vel.y_ = GetRand(-0.2f, 0.2f);
		vel.z_ = GetRand(-0.2f, 0.2f);

		float scale = GetRand(GetScale().x_ / 2.0f, GetScale().x_ * 4.0f);

		ParticleManager::GetInstance()->Add(30, interPos, vel, { 0,0,0 }, scale, 0, { 0,0,0,1.5f }, { 0,0,0,0.0f });
	}
}

void Gun::OnCollision(const CollisionInfo& info)
{
	if (info.object_->GetObjName() == "bullet")
	{
		//�p�[�e�B�N��
		ParticleGenerate({ 0,0,0,1.5f }, { 0,0,0,0 });

		//�����傪�܂������玝����̕���|�C���^��nullptr��
		if (owner_)
		{
			Character* character = dynamic_cast<Character*>(owner_);
			character->FallWeapon({ 0,0,0 });
		}

		SetIsAlive(false);
	}
}
