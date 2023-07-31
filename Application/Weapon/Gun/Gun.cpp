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

	//初期化
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

	//ローカル座標
	localPos_ = GetTrans();

	return true;
}

void Gun::Attack(const Vec3& directionVec, int32_t decreBullet, Object* owner)
{
	//クールタイムじゃなく、残弾なかったら抜ける
	if (shotCoolTime_ > 0 || remainingBullets_ <= 0)
	{
		return;
	}

	//発射座標(銃本体に当たらないようにする)
	shotPos_ = { GetWorldTrans().x_ + directionVec.GetNormalized().x_ * GetScale().x_ * 1.1f,
		GetWorldTrans().y_ + GetScale().y_,
		GetWorldTrans().z_ + directionVec.GetNormalized().z_ * GetScale().z_ * 1.1f };

	//弾うつ処理
	BulletManager::GetInstance().CreateBullet(shotPos_, directionVec.GetNormalized() * BULLET_VELOCITY_, GetScale().x_ * 0.4f, 300, owner);

	//パーティクル
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
	//親が亡くなった後の動き
	NoParentMove();
	//地面より下に行かないように
	if (GetParent() == nullptr)
	{
		SetTransY(max(GetTrans().y_, GetScale().x_));
	}

	//クールタイムもゲームスピードをかける
	shotCoolTime_ -= 1.0f * GameVelocityManager::GetInstance().GetVelocity();

	Object::Update();
}


//----------------------------------------------------------------------------------------------------------------
void Gun::ParticleGenerate(const XMFLOAT4& sColor, const XMFLOAT4& eColor)
{
	//パーティクル
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

	//パーティクル
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
		//パーティクル
		ParticleGenerate({ 0,0,0,1.5f }, { 0,0,0,0 });

		//持ち主がまだいたら持ち主の武器ポインタをnullptrに
		if (owner_)
		{
			Character* character = dynamic_cast<Character*>(owner_);
			character->FallWeapon({ 0,0,0 });
		}

		SetIsAlive(false);
	}
}
