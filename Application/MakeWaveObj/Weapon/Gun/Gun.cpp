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
#include "ObjectManager.h"
#include "LevelManager.h"

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
		assert(false);
	}

	return std::move(instance);
}

float Gun::GetAttackCoolTimeRatio()
{
	return TIME_RATIO_MAX_ - min((float)max(attackCoolTime_, 0.0f) / (float)SHOT_COOL_TIME_MAX_, TIME_RATIO_MAX_);
}

bool Gun::Initialize(std::unique_ptr<WorldMat> worldMat, IModel* model)
{
	if (!Object::Initialize(std::move(worldMat)))
	{
		return false;
	}

	//アンビエントなどの倍率
	model->SetMaterialExtend(MATERIAL_EXTEND_);

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

	Vec3 trans = boneWorldTrans_;
	if (trans.GetLength() == 0)
	{
		trans = GetWorldTrans();
	}

	//発射座標(銃本体に当たらないようにする)
	shotPos_ = { trans.x + directionVec.GetNormalized().x * GetScale().x * shotPosExtend_,
				 trans.y + directionVec.GetNormalized().y * GetScale().y * shotPosExtend_,
				 trans.z + directionVec.GetNormalized().z * GetScale().z * shotPosExtend_ };

	//弾うつ処理
	BulletManager::GetInstance().CreateBullet(shotPos_, directionVec.GetNormalized() * BULLET_VELOCITY_, GetScale().x * BULLET_SCALE_RATE_, BULLET_LIFE_TIME_, owner);

	//パーティクル
	ParticleGenerate(SHOOT_PARTICLE_START_COLOR_, SHOOT_PARTICLE_END_COLOR_, particleSize);

	//ステージに波紋
	BeginWaveStage(shotPos_, Vec2(GetScale().z, GetScale().y) * SHOOT_STAGE_WAVE_THICK_EXTEND_RATE_1_, GetScale().GetLength() * SHOOT_STAGE_WAVE_DISTANCE_RATE_,
		SHOOT_STAGE_WAVE_TIME_1_);
	BeginWaveStage(shotPos_, Vec2(GetScale().z, GetScale().y) * SHOOT_STAGE_WAVE_THICK_EXTEND_RATE_2_, GetScale().GetLength() * SHOOT_STAGE_WAVE_DISTANCE_RATE_,
		SHOOT_STAGE_WAVE_TIME_2_);

	attackCoolTime_ = SHOT_COOL_TIME_MAX_;
	remainingBullets_ -= decreBullet;
}

void Gun::ChangeOwner(IObject3D* parent)
{
	remainingBullets_ = BULLETS_TMP_;
	attackCoolTime_ = 0;

	boneWorldTrans_ = { 0,0,0 };

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
	attackCoolTime_ -= GameVelocityManager::GetInstance().GetVelocity();

	Weapon::Update();
}

//----------------------------------------------------------------------------------------------------------------
void Gun::ParticleGenerate(const XMFLOAT4& sColor, const XMFLOAT4& eColor, float particleSize, ParticleManager::BLEND_NUM blendNum)
{
	//パーティクル
	for (int32_t i = 0; i < PARTICLE_NUM_; ++i)
	{
		Vec3 vel{};
		vel.x = GetRand(PARTICLE_VEL_MIN_, PARTICLE_VEL_MAX_);
		vel.y = GetRand(PARTICLE_VEL_MIN_, PARTICLE_VEL_MAX_);
		vel.z = GetRand(PARTICLE_VEL_MIN_, PARTICLE_VEL_MAX_);

		float scale = GetRand(GetScale().x / 2.0f, GetScale().x * 2.0f);

		ParticleManager::GetInstance()->Add(PARTICLE_LIFE_TIME_, shotPos_, vel * PARTICLE_VEL_RATE_, { 0,0,0 }, scale * particleSize, 0, sColor, eColor,
			blendNum);
	}
}

void Gun::OnLandShape(const Vec3& interPos)
{
	SetIsAlive(false);

	//ステージに波紋
	float waveLength = GetScale().GetLength();
	for (int i = 0; i < ON_STAGE_WAVE_NUM_; i++)
	{
		BeginWaveStage(GetTrans(), Vec2(waveLength, waveLength) * ON_STAGE_WAVE_THICK_EXTEND_RATE_,
			waveLength * GetRand(ON_STAGE_WAVE_DISTANCE_RATE_MIN_, ON_STAGE_WAVE_DISTANCE_RATE_MAX_), ON_STAGE_WAVE_TIME_);
	}

	//パーティクル
	for (int32_t i = 0; i < ON_STAGE_PARTICLE_NUM_; ++i)
	{
		Vec3 vel{};
		vel.x = GetRand(ON_STAGE_PARTICLE_VEL_MIN_, ON_STAGE_PARTICLE_VEL_MAX_);
		vel.y = GetRand(ON_STAGE_PARTICLE_VEL_MIN_, ON_STAGE_PARTICLE_VEL_MAX_);
		vel.z = GetRand(ON_STAGE_PARTICLE_VEL_MIN_, ON_STAGE_PARTICLE_VEL_MAX_);

		float scale = GetRand(GetScale().x * ON_STAGE_PARTICLE_SCALE_RATE_MIN_, GetScale().x * ON_STAGE_PARTICLE_SCALE_RATE_MAX_);

		ParticleManager::GetInstance()->Add(ON_STAGE_PARTICLE_LIFE_TIME_, interPos, vel, { 0,0,0 }, scale, 0, ON_STAGE_PARTICLE_START_COLOR_, ON_STAGE_PARTICLE_END_COLOR_,
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
			character->FallHaveWeapon({ 0,0,0 });
		}
	}
}
