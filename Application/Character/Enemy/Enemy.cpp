#include "Enemy.h"
#include "KeyboardInput.h"
#include "DebugText.h"
#include "CameraManager.h"
#include "ParticleManager.h"
#include "PadInput.h"
#include "MouseInput.h"
#include "GameVelocityManager.h"
#include "EnemyState.h"
#include "LevelManager.h"
#include "Bullet.h"

using namespace DirectX;


const float Enemy::S_LENGTH_MAX_ = 10000;


Enemy::~Enemy()
{
}

void Enemy::EmergeInitialize()
{
	LightManager* lightM = LevelManager::GetInstance().GetLightManager();

	//使えるライトがあれば
	if (lightM->GetDoNotUsePointLightIndex(lightIndexTmp_))
	{
		lightM->SetPointLightActive(lightIndexTmp_, true);
		lightM->SetPointLightAtten(lightIndexTmp_, { 0.977f,0.493f,0.458f });
		lightM->SetPointLightColor(lightIndexTmp_, { 5.0f,0,0 });
		lightM->SetPointLightPos(lightIndexTmp_,
			{ IObject::GetTrans().x,IObject::GetTrans().y, IObject::GetTrans().z });
	}
}

void Enemy::ChangeEnemyState(std::unique_ptr<EnemyState> state)
{
	state_.reset();
	state_ = std::move(state);
	state_->SetEnemy(this);
	state_->Initialize();
}


std::unique_ptr<Enemy> Enemy::Create(std::unique_ptr<WorldMat> worldMat, int32_t waveNum, Weapon* weapon, IModel* model)
{
	std::unique_ptr<Enemy> instance = std::make_unique<Enemy>();
	if (instance.get() == nullptr)
	{
		return nullptr;
	}

	//初期化
	if (!instance->Initialize(std::move(worldMat), waveNum, weapon, model))
	{
		assert(0);
	}

	return std::move(instance);
}

bool Enemy::Initialize(std::unique_ptr<WorldMat> worldMat, int32_t waveNum, Weapon* weapon, IModel* model)
{
	if (!ObjectFBX::Initialize(std::move(worldMat)))
	{
		return false;
	}


	if (weapon)
	{
		weapon_ = weapon;
		//武器を諸々設定
		PickUpWeapon(weapon_);
	}

	hp_ = HP_TMP_;

	waveNum_ = waveNum;

	//model
	model->SetMaterialExtend({ 1.0f,3.0f,140.0f });

	//ディゾルブ
	SetisDissolve(true);
	SetDissolveT(1.0f);
	//ディゾルブ画像
	uint64_t handle;
	handle = TextureManager::LoadGraph("dissolveMask.png");
	SetDissolveTexHandle(handle);

	//ステート変更
	ChangeEnemyState(std::make_unique<EnemyStateEmergeEffect>());

	//アニメーション開始
	PlayAnimation(true);

	//色
	SetColor({ 3.0f,0,0,1.0f });

	//演出更新
	ObjectFBX::EffectUpdate();
	//fbx前提
	ModelFBX* modelFbx = dynamic_cast<ModelFBX*>(model);
	//ノードごとの当たり判定
	nodeColliders_.SetNodesPointer(modelFbx->GetNodes());
	nodeColliders_.CreateNodeColliders();
	nodeColliders_.SetNodeParamMagnif(modelFbx->GetScaleExtend());
	nodeColliders_.SetScaleCollider(3.0f);

	return true;
}

void Enemy::AllMove(const Vec3& targetPos)
{
	WalkToTarget(targetPos);

	CollisionWallAndFloor();
}

void Enemy::Attack(const Vec3& targetPos)
{
	//武器で攻撃
	if (weapon_ != nullptr && weapon_->GetIsAlive())
	{
		Vec3 directionV = targetPos - weapon_->GetWorldTrans();
		weapon_->Attack(directionV.GetNormalized(), 0, this, PARTICLE_SIZE_EXTEND_);
	}
}

void Enemy::WalkToTarget(const Vec3& targetPos)
{
	//動けなかったら飛ばす
	if (isCantMove)
	{
		isCantMove = false;
		return;
	}

	//正規化
	directionVec_.y = 0;
	directionVec_.Normalized();

	//現在のスピードに方向ベクトル足し、ゲームスピードをかける
	velocity_ = GetVelocity() + directionVec_ * VELOCITY_TMP_ * GameVelocityManager::GetInstance().GetVelocity();
	//スピードの上限は超えないように
	float length = velocity_.GetLength();
	//スピードがプラスになってたら
	if (velocity_.Dot(directionVec_) >= 0)
	{
		directionVec_ = targetPos - IObject::GetTrans();
		//ある程度近づいたら止まる
		if (directionVec_.GetLength() < IObject::GetScale().GetLength() * 1.5f)
		{
			velocity_ = { 0,0,0 };
		}
		directionVec_.Normalized();

		//ダメージのクールリセット
		damageCoolTime_ = 0;
	}
	//スピードの上限超えない
	velocity_ = velocity_.GetNormalized() * min(fabsf(length), GameVelocityManager::GetInstance().GetVelocity() * VELOCITY_TMP_ / 1.5f);

	//当たり判定用にセット
	IObject::SetVelocity(velocity_);

	//位置セット
	IObject::SetTrans(IObject::GetTrans() + velocity_);

	//向き変更
	DirectionUpdate(targetPos);
}

void Enemy::CollisionWallAndFloor()
{
	//地面と壁との判定
	OnGroundAndWallUpdate(IObject::GetScale().y, GameVelocityManager::GetInstance().GetVelocity());
}

void Enemy::DirectionUpdate(const Vec3& targetPos)
{
	//敵からターゲットへのベクトル
	directionVec_ = targetPos - GetTrans();
	//初期正面ベクトルとターゲットへのベクトル
	Vec3 fVTmp = GetFrontVecTmp().GetNormalized();
	Vec3 pDVTmp = directionVec_.GetNormalized();

	//正面ベクトルからターゲットの方向ベクトルへの回転クォータニオン
	Quaternion q = Quaternion::DirectionToDirection(fVTmp, pDVTmp, 0.0f);
	//回転後のベクトル
	fVTmp = q.GetRotateVector(fVTmp);
	//正面ベクトルセット
	IObject3D::SetFrontVec(fVTmp);

	//角度じゃなくて行列をそのまま使う
	IObject::SetIsUseQuaternionMatRot(true);
	IObject::SetMatRot(q.MakeRotateMatrix());
}

void Enemy::HPUpdate()
{
	//hpによってディゾルブ
	IObject3D::SetDissolveT((1.0f - (float)hp_ / (float)HP_TMP_) * DISSOLVE_POW_);

	//ポリゴンごとに動くように
	Mesh::PolygonOffset offsetData;
	offsetData.interval = GetRand(15.0f, 35.0f) * (1.0f - GameVelocityManager::GetInstance().GetVelocity() * 1.5f);
	offsetData.length = GetRand(-IObject::GetScale().x, IObject::GetScale().x) * 5.5f * max(1.0f - (float)hp_ / (float)HP_TMP_, 0);
	offsetData.ratio = (1.0f - (float)hp_ / (float)HP_TMP_);
	ObjectFBX::SetMeshPolygonOffsetData(offsetData);
}

//----------------------------------------------------------------
void Enemy::Update()
{
	//ダメージ受けるクールタイムもゲームスピードをかける
	damageCoolTime_ -= 1.0f * GameVelocityManager::GetInstance().GetVelocity();

	//アニメーションもゲームスピード
	SetAnimationSpeed(min(GameVelocityManager::GetInstance().GetVelocity() * 3.0f,
		GameVelocityManager::GetInstance().GAME_VELOCITY_MAX_));

	//演出更新
	ObjectFBX::EffectUpdate();

	//ステート
	state_->Update();

	Character::Update();

	//ノードごとの当たり判定
	nodeColliders_.Update(GetWorldMat());
}

void Enemy::Draw()
{
	//ObjectFBX::DrawModel(nullptr);

	nodeColliders_.Draw();
}


//----------------------------------------------------------------------------------
void Enemy::KnockBack(const CollisionInfo& info)
{
	//長さ
	float length = (info.object_->GetScale().z + IObject::GetScale().z);
	//距離のベクトル
	Vec3 distanceVec = IObject::GetTrans() - info.object_->GetWorldTrans();
	//仮
	distanceVec.y = 0;
	velocity_.y = 0;
	distanceVec.Normalized();
	//ノックバック
	velocity_ += distanceVec * length * KNOCK_BACK_POW_;
	IObject::SetVelocity(velocity_);
	//ダメージを受けるクールタイム
	damageCoolTime_ = 20;

	//武器持っていたら落とす
	if (weapon_)
	{
		distanceVec.y = 0.2f;
		FallWeapon(Vec3(-distanceVec.x, distanceVec.y, -distanceVec.z) * length);
	}
}

void Enemy::DamageParticle(const CollisionInfo& info, const Vec3& offsetPosExtend, int32_t particleNum)
{
	for (int32_t i = 0; i < particleNum; ++i)
	{
		Vec3 pos = { info.inter_.m128_f32[0],info.inter_.m128_f32[1],info.inter_.m128_f32[2] };

		float scaleTmp = IObject::GetScale().GetLength();

		Vec3 addPos = Vec3(GetRand(-IObject::GetScale().x, IObject::GetScale().x) * offsetPosExtend.x,
			GetRand(-IObject::GetScale().y, IObject::GetScale().y) * offsetPosExtend.y,
			GetRand(-IObject::GetScale().z, IObject::GetScale().z) * offsetPosExtend.z);

		pos += addPos;

		const int32_t LIFE_TIME = 40;

		//相手の速度も使う
		Vec3 infoVec = info.object_->GetVelocity().GetNormalized();

		Vec3 vel = Vec3(infoVec.x * GetRand(-0.1f, 1.0f),
			infoVec.y * GetRand(-0.1f, 1.0f) + GetRand(0, IObject::GetScale().y / 8.0f),
			infoVec.z * GetRand(-0.1f, 1.0f));

		float scale = scaleTmp / 30.0f;
		float scale2 = 0;

		ParticleManager::GetInstance()->Add(LIFE_TIME, pos, vel, { 0,-0.002f,0 }, scale, scale2, { 3.0f,0.02f,0.02f,0.95f }, { 3.0f,0.02f,0.02f,0.95f },
			PI * 2.0f, -PI * 2.0f);
	}
}

void Enemy::OnCollision(const CollisionInfo& info)
{
	//プレイヤーに当たったら
	if (info.object_->GetObjName() == "player")
	{
		////長さ
		float length = (info.object_->GetScale().x + IObject::GetScale().x);
		//距離のベクトル
		Vec3 distanceVec = IObject::GetTrans() - info.object_->GetTrans();
		//仮でyは動かさない
		distanceVec.y = 0;
		distanceVec.Normalized();

		//めり込まないように位置セット(半径＋半径の長さをベクトルの方向を使って足す)
		Vec3 ansPosE = info.object_->GetTrans() + distanceVec * length * 1.001f;
		IObject::SetTrans(ansPosE);

		IObject::SetVelocity({ 0,0,0 });

		//動けないようにする
		isCantMove = true;
	}
	//プレイヤーの攻撃との判定
	else if (info.object_->GetObjName().find("hand") != std::string::npos)
	{
		if (damageCoolTime_ <= 0)
		{
			//ノックバック
			KnockBack(info);

			//hp減らす
			Damaged(1, [=]() { IObject::SetIsAlive(false); });

			//パーティクル
			DamageParticle(info);
		}
	}
	//弾に当たったら
	else if (info.object_->GetObjName() == "bullet")
	{
		//弾を撃った本人だったら
		if (this == dynamic_cast<Bullet*>(info.object_)->GetOwner())
		{
			return;
		}

		//今のhp分ダメージ受けて倒れる
		Damaged(hp_, [=]() {IObject::SetIsAlive(false); });

		//ノックバック
		KnockBack(info);

		//パーティクル
		DamageParticle(info, { 0.5f,1.0f,0.5f }, 400);
	}
	//銃に当たったら
	else if (info.object_->GetObjName() == "gun")
	{
		Gun* gun = dynamic_cast<Gun*>(info.object_);

		if (!(gun != nullptr && gun->GetParent() == nullptr))
		{
			return;
		}

		//投げられているときのみ
		if (gun->GetIsThrowing() && gun->GetFallVelocity().GetLength() != 0)
		{
			//
			KnockBack(info);
		}
		//おいてあったら拾う
		else
		{
			PickUpWeapon(gun);
		}
	}
	//敵同士で当たったらめり込まないようにする
	else if (info.object_->GetObjName().find("enemy") != std::string::npos)
	{
		//長さ
		float length = (info.object_->GetScale().x + IObject::GetScale().x);
		//距離のベクトル
		Vec3 distanceVec = IObject::GetTrans() - info.object_->GetTrans();
		//仮
		distanceVec.y = 0;
		distanceVec.Normalized();
		//位置セット(半径＋半径の長さをベクトルの方向を使って足す)
		IObject::SetTrans(info.object_->GetTrans() + distanceVec * length * 1.001f);

		//ｙは動かないようにする
		IObject::SetVelocity({ IObject::GetVelocity().x,0,IObject::GetVelocity().z });
		info.object_->SetVelocity({ info.object_->GetVelocity().x,0,info.object_->GetVelocity().z });
		//二つのベクトルの合計の長さ
		float addLength = GetVelocity().GetLength() + info.object_->GetVelocity().GetLength();
		//自分のスピードのベクトルの長さの割合（合計の長さで割る（0～1.0f））
		float myLengthRatio = GetVelocity().GetLength() / addLength;

		//衝突後の自分のスピードベクトルは[現在のスピードベクトル]+[相手から自分へのベクトル]*[相手の長さの割合]
		SetVelocity((GetVelocity() + distanceVec.GetNormalized() * addLength * (1.0f - myLengthRatio)) * 0.63f);
		//衝突後の相手のスピードベクトルは[現在のスピードベクトル]+[このインスタンスから相手へのベクトル]*[このインスタンスの長さの割合]
		info.object_->SetVelocity((info.object_->GetVelocity() - distanceVec.GetNormalized() * addLength * (myLengthRatio)) * 0.63f);
	}
}