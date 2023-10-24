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


std::unique_ptr<Gun> Gun::Create(std::unique_ptr<WorldMat> worldMat, IModel* model)
{
	std::unique_ptr<Gun> instance = std::make_unique<Gun>();
	if (instance.get() == nullptr)
	{
		return nullptr;
	}

	//初期化
	if (!instance->Initialize(std::move(worldMat), model))
	{
		assert(0);
	}

	return std::move(instance);
}

float Gun::GetAttackCoolTimeRatio()
{
	return 1.0f - min((float)max(attackCoolTime_, 0.0f) / (float)SHOT_COOL_TIME_MAX_, 1.0f);
}

bool Gun::Initialize(std::unique_ptr<WorldMat> worldMat, IModel* model)
{
	if (!Object::Initialize(std::move(worldMat)))
	{
		return false;
	}

	//アンビエントなどの倍率
	model->SetMaterialExtend({ 1.0f,100.0f,5000.0f });

	//ローカル座標
	localPos_ = GetTrans();

	return true;
}

void Gun::Attack(const Vec3& directionVec, int32_t decreBullet, IObject3D* owner, float particleSize)
{
	//クールタイムがまだあったり、残弾なかったら抜ける
	if (attackCoolTime_ > 0 || remainingBullets_ <= 0)
	{
		return;
	}

	Vec3 trans = worldTrans_;
	if (trans.GetLength() == 0)
	{
		trans = GetWorldTrans();
	}

	//発射座標(銃本体に当たらないようにする)
	shotPos_ = { trans.x + directionVec.GetNormalized().x * GetScale().x * 1.1f,
				 trans.y + GetScale().y,
				 trans.z + directionVec.GetNormalized().z * GetScale().z * 1.1f };

	//弾うつ処理
	BulletManager::GetInstance().CreateBullet(shotPos_, directionVec.GetNormalized() * BULLET_VELOCITY_, GetScale().x * 0.4f, 300, owner);

	//パーティクル
	ParticleGenerate({ 4.0f,4.0f,4.0f,1.5f }, { 4.0f,4.0f,4.0f,0 }, particleSize);

	attackCoolTime_ = SHOT_COOL_TIME_MAX_;
	remainingBullets_ -= decreBullet;
}

void Gun::ChangeOwner(IObject3D* parent)
{
	remainingBullets_ = BULLETS_TMP_;
	attackCoolTime_ = 0;

	Weapon::ChangeOwner(parent);
}

void Gun::Update()
{
	//親が亡くなった後の動き
	NoParentMove();
	//地面より下に行かないように
	if (GetParent() == nullptr)
	{
		SetTransY(max(GetTrans().y, GetScale().x));
	}

	//クールタイムもゲームスピードをかける
	attackCoolTime_ -= 1.0f * GameVelocityManager::GetInstance().GetVelocity();

	Weapon::Update();
}

void Gun::Draw()
{
	Object::DrawModel(nullptr);
}


//----------------------------------------------------------------------------------------------------------------
void Gun::ParticleGenerate(const XMFLOAT4& sColor, const XMFLOAT4& eColor, float particleSize, ParticleManager::BLEND_NUM blendNum)
{
	//パーティクル
	for (int32_t i = 0; i < 50; ++i)
	{
		Vec3 vel{};
		vel.x = GetRand(-0.5f, 0.5f);
		vel.y = GetRand(-0.5f, 0.5f);
		vel.z = GetRand(-0.5f, 0.5f);

		float scale = GetRand(GetScale().x / 2.0f, GetScale().x * 2.0f);

		ParticleManager::GetInstance()->Add(20, shotPos_, vel * 0.5f, { 0,0,0 }, scale * particleSize, 0, sColor, eColor,
			blendNum);
	}

}

void Gun::OnLandShape(const Vec3& interPos)
{
	SetIsAlive(false);

	//パーティクル
	for (int32_t i = 0; i < 20; ++i)
	{
		Vec3 vel{};
		vel.x = GetRand(-0.2f, 0.2f);
		vel.y = GetRand(-0.2f, 0.2f);
		vel.z = GetRand(-0.2f, 0.2f);

		float scale = GetRand(GetScale().x / 2.0f, GetScale().x * 4.0f);

		ParticleManager::GetInstance()->Add(30, interPos, vel, { 0,0,0 }, scale, 0, { 0.001f,0.001f,0.01f,1.5f }, { 0.1f,0.1f,0.1f,1.0f },
			ParticleManager::BLEND_NUM::CRYSTAL);
	}
}

void Gun::OnCollision(const CollisionInfo& info)
{
	if (info.object_->GetObjName() == "bullet")
	{
		//パーティクルと死亡フラグ
		OnLandShape({ info.inter_.m128_f32[0],info.inter_.m128_f32[1] ,info.inter_.m128_f32[2] });

		//持ち主がまだいたら持ち主の武器ポインタをnullptrに
		if (owner_)
		{
			Character* character = dynamic_cast<Character*>(owner_);
			character->FallWeapon({ 0,0,0 });
		}
	}
}
