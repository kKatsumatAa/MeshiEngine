#include "EnemyPart.h"
#include "MeshCollider.h"
#include "GameVelocityManager.h"
#include "StageManager.h"
#include "StageState.h"


const std::string EnemyPart::S_OBJ_NAME_ = "enemyPart";

//---------------------------------
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

	//名前
	SetObjName(S_OBJ_NAME_);

	// メッシュコライダーの追加
	std::unique_ptr<MeshCollider> collider = std::make_unique<MeshCollider>();
	collider->ConstructTriangles(model);
	SetCollider(std::move(collider));

	return true;
}

//----------------------------------------------------------------
void EnemyPart::Update()
{
	Weapon::Update();

	NoParentMove();

	//死亡
	if (GetTrans().y <= -StageManager::GetInstance().SEA_DICTANCE_TMP_)
	{
		SetIsAlive(false);
	}
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
	if (info.object_ == nullptr)
	{
		return;
	}
}