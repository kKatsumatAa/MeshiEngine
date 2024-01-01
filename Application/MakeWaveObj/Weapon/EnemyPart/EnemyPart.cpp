#include "EnemyPart.h"

std::unique_ptr<EnemyPart> EnemyPart::Create(const WorldMat& worldMat, IModel* model)
{
	std::unique_ptr<EnemyPart> instance = std::make_unique<EnemyPart>();
	if (instance.get() == nullptr)
	{
		return nullptr;
	}

	//パラメータコピー
	auto lWorld = std::make_unique<WorldMat>();
	lWorld->trans_ = worldMat.trans_;
	lWorld->scale_ = worldMat.scale_;
	lWorld->rot_ = worldMat.rot_;

	//初期化
	if (!instance->Initialize(std::move(lWorld), model))
	{
		assert(false);
	}

	return std::move(instance);
}

bool EnemyPart::Initialize(std::unique_ptr<WorldMat> worldMat, IModel* model)
{
	if (worldMat == nullptr)
	{
		return false;
	}

	//ローカル座標
	localPos_ = GetTrans();
	//譲渡
	worldMat_ = std::move(worldMat);

	//モデル
	SetModel(model);

	return true;
}

//----------------------------------------------------------------
void EnemyPart::Update()
{
	NoParentMove();
}

//-----------------------------------------------------------------------------------------------------------------------------------------
void EnemyPart::OnLandShape(const Vec3& interPos)
{
	interPos;
}

void EnemyPart::ParticleGenerate(const XMFLOAT4& sColor, const XMFLOAT4& eColor, float particleSize, ParticleManager::BLEND_NUM blendNum)
{
	sColor; eColor; particleSize; blendNum;
}

void EnemyPart::Attack(const Vec3& directionVec, int32_t decreBullet, IObject3D* owner, float particleSize)
{
	directionVec; decreBullet; owner; particleSize;
}

float EnemyPart::GetAttackCoolTimeRatio()
{
	return 0.0f;
}

void EnemyPart::OnCollision(const CollisionInfo& info)
{
	info;
}