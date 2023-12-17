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
#include "ObjectManager.h"
#include "Util.h"
#include "FbxLoader.h"

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
		lightM->SetPointLightColor(lightIndexTmp_, { 90.0f,-1.0f,-1.0f });
		lightM->SetPointLightPos(lightIndexTmp_,
			{ IObject::GetTrans().x,IObject::GetTrans().y, IObject::GetTrans().z });
	}

	//ディゾルブ
	SetisDissolve(true);
	SetDissolveT(1.0f);
	//ディゾルブ画像
	uint64_t handle;
	handle = TextureManager::LoadGraph("dissolveMask.png");
	SetDissolveTexHandle(handle);

	//演出更新
	ObjectFBX::EffectUpdate();

	//fbx前提
	ModelFBX* modelFbx = dynamic_cast<ModelFBX*>(GetModel());
	//ノードごとの当たり判定
	InitializeNodeColliders(modelFbx, 14.0f, COLLISION_ATTR_ENEMYS);
	//ボーンの当たり判定時に処理させる
	auto onCollF = [=](IObject3D* obj, const CollisionInfo& info) {OnCollision(obj, info); };
	SetNodeCollidersOnCollision(onCollF);

	//ステート変更
	ChangeEnemyState(std::make_unique<EnemyStateEmergeEffect>());
}

void Enemy::ChangeEnemyState(std::unique_ptr<EnemyState> state)
{
	state_.reset();
	state_ = std::move(state);
	state_->SetEnemy(this);
	state_->Initialize();
}

void Enemy::ChangeEnemyStanceState(std::unique_ptr<EnemyState> state)
{
	stanceState_.reset();
	stanceState_ = std::move(state);
	stanceState_->SetEnemy(this);
	stanceState_->Initialize();
}


//--------------------------------------------------------------------------------------------------------------------------
std::unique_ptr<Enemy> Enemy::Create(std::unique_ptr<WorldMat> worldMat, int32_t waveNum, float coolTime, Weapon* weapon, IModel* model)
{
	std::unique_ptr<Enemy> instance = std::make_unique<Enemy>();
	if (instance.get() == nullptr)
	{
		return nullptr;
	}

	//初期化
	if (!instance->Initialize(std::move(worldMat), waveNum, coolTime, weapon, model))
	{
		assert(0);
	}

	return std::move(instance);
}

bool Enemy::Initialize(std::unique_ptr<WorldMat> worldMat, int32_t waveNum, float coolTime, Weapon* weapon, IModel* model)
{
	if (!ObjectFBX::Initialize(std::move(worldMat)))
	{
		return false;
	}


	shotPosExtend_ = SHOT_POS_EXTEND_;
	if (weapon)
	{
		weapon_ = weapon;
		weapon_->SetScale(weapon_->GetScale() * 2.0f);
		//武器を諸々設定
		PickUpWeapon(weapon_);
	}

	//hpセット
	hp_ = HP_TMP_;
	oldHP_ = hp_;

	waveNum_ = waveNum;
	coolTime_ = (float)coolTime;

	//死亡時のタイマー上限
	deadTimerMax_ = DEAD_TIMER_MAX_;

	//model
	model->SetMaterialExtend({ 0.03f,0.1f,50.0f });

	//色
	SetColor({ 3.0f,0,0,1.0f });

	//アニメーション追加
	auto modelFbx = dynamic_cast<ModelFBX*>(model);
	FbxLoader::GetInstance()->AddAnimationModel(modelFbx, "punch");

	//モデル
	SetModel(modelFbx);

	//アニメーション開始
	PlayAnimation(true, WALK);

	//分割数
	SetTessFactor(TESS_FACTOR_MAX_);

	return true;
}

void Enemy::AllMove(const Vec3& targetPos, bool isWave)
{
	WalkToTarget(targetPos, isWave);

	CollisionWallAndFloor();
}

void Enemy::Attack(const Vec3& targetPos)
{
	//武器で攻撃
	if (weapon_ != nullptr && weapon_->GetIsAlive())
	{
		Vec3 directionV = targetPos - weapon_->GetBoneWorldTrans();
		weapon_->Attack(directionV.GetNormalized(), 0, this, PARTICLE_SIZE_EXTEND_);
	}
}

void Enemy::WalkToTarget(const Vec3& targetPos, bool isWave)
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
		if (GetPlayerIsWithinRange())
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
	IObject::SetTrans(IObject::GetTrans() + velocity_ * animeSpeedExtend_);

	//向き変更
	DirectionUpdate(targetPos);

	//波紋発生
	if (isWave)
	{
		WalkWaveUpdate();
	}
}

//---------------------------------------------------------------------------------------------
void Enemy::SetAllNodeAddRots(const IObject& nodeObj, float angleExtend)
{
	damagedAddRots_.clear();

	Vec3 addRot = { 0,0,0 };
	//Spine2(仮)からの距離で回転方向を決めるため(回転の逆行列をかけて元の位置関係に戻したうえで)
	Vec3 localPosFromSpine2 = nodeObj.GetLocalTrans() * GetWorldMat()->GetRotMat().GetInverseMatrix()
		- ObjectFBX::GetNodeColliderObj("Spine2")->GetLocalTrans() * GetWorldMat()->GetRotMat().GetInverseMatrix();

	addRot.x = max(min(-localPosFromSpine2.y, PI / 6.0f), -PI / 6.0f) * angleExtend;
	addRot.y = max(min(-localPosFromSpine2.x, PI / 6.0f), -PI / 6.0f) * angleExtend;

	//右か左か
	std::string leftOrRightStr = "";
	if (nodeObj.GetObjName().find("Right") != std::string::npos)
	{
		leftOrRightStr = "Right";
	}
	else if (nodeObj.GetObjName().find("Left") != std::string::npos)
	{
		leftOrRightStr = "Left";
	}

	//足に攻撃されたら
	if (nodeObj.GetObjName().find("Toe") != std::string::npos ||
		nodeObj.GetObjName().find("Foot") != std::string::npos ||
		nodeObj.GetObjName().find("Leg") != std::string::npos)
	{
		//Spine2(仮)からの距離で回転方向を決めるため
		Vec3 localPosFromLeg = nodeObj.GetLocalTrans() - ObjectFBX::GetNodeColliderObj("UpLeg")->GetLocalTrans();

		Vec3 addRotLeg = { 0,0,0 };
		addRotLeg.x = max(min(localPosFromLeg.y, PI / 5.5f), -PI / 5.5f) * angleExtend;
		addRotLeg.y = max(min(-localPosFromLeg.x, PI / 5.5f), -PI / 5.5f) * angleExtend;

		damagedAddRots_.push_back({ leftOrRightStr + "Leg", {0,0,0},addRotLeg });
	}

	damagedAddRots_.push_back({ "Spine2", {0,0,0},addRot });
}

void Enemy::WalkWaveUpdate()
{
	float gameVel = GameVelocityManager::GetInstance().GetVelocity() * GetAnimeSpeedExtend();

	walkWaveTimer_ += gameVel;
	if (walkWaveTimer_ >= WALK_MOVE_INTERVAL_ * 500.0f)
	{
		walkWaveTimer_ = 0;
	}

	//間隔が来たらステージに波紋
	if ((int32_t)walkWaveTimer_ % (int32_t)WALK_MOVE_INTERVAL_ == 0
		&& walkWaveTimer_ - beforeWalkTime_ >= WALK_MOVE_INTERVAL_)
	{
		BeginWaveStage(GetWorldTrans() - Vec3(0, GetScale().y, 0), { GetScale().z / 2.0f,GetScale().y * 0.2f }, GetScale().z * 20.0f, 130.0f);
		//間隔をあけるため
		beforeWalkTime_ = walkWaveTimer_;
	}
}

void Enemy::Dead()
{
	ChangeEnemyState(std::make_unique<EnemyStateDead>());
}

//---------------------------------------------------------------------------------------------
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

void Enemy::HPUpdate(float t)
{
	//ディゾルブ強すぎると敵が見えなくなるので
	float lDissolvePow = DISSOLVE_POW_;
	if (hp_ <= 0.0f)
	{
		lDissolvePow = Lerp(DISSOLVE_POW_, 1.0f, EaseOut(t));
	}
	//hpによってディゾルブ(減った後のhpとの線形補完)
	IObject3D::SetDissolveT((1.0f - Lerp((float)oldHP_, (float)hp_, t) / (float)HP_TMP_) * lDissolvePow);

	//ポリゴンごとに動くように
	Mesh::PolygonOffset offsetData;
	offsetData.interval = GetRand(15.0f, 35.0f) * (1.0f - GameVelocityManager::GetInstance().GetVelocity() * 1.1f);
	offsetData.length = GetRand(-IObject::GetScale().x, IObject::GetScale().x) * 1.3f;
	offsetData.ratio = (1.0f - (float)hp_ / (float)HP_TMP_);
	ObjectFBX::SetMeshPolygonOffsetData(offsetData);
}

void Enemy::BeginDamagedWave(const CollisionInfo& info, float wavePow)
{
	Vec3 pos = { info.inter_.m128_f32[0] ,info.inter_.m128_f32[1] ,info.inter_.m128_f32[2] };

	//自分にメッシュの波
	for (int i = 0; i < 3; i++)
	{
		BeginWave(pos, { GetScale().y / 20.0f * wavePow,GetScale().GetLength() / 18.0f * wavePow },
			GetScale().GetLength() * 2.0f, 45.0f / wavePow * GetRand(1.0f, 2.0f));
	}

	//ステージに波紋
	for (int i = 0; i < 2; i++)
	{
		BeginWaveStage(pos, { GetScale().y * wavePow,GetScale().GetLength() * wavePow },
			GetScale().GetLength() * 12.0f, 15.0f * GetRand(0.5f, 2.0f));
	}
}

bool Enemy::GetPlayerIsWithinRange()
{
	auto players = ObjectManager::GetInstance().GetObjs(LevelManager::S_OBJ_GROUP_NAME_, "player");

	for (auto player : players)
	{
		Vec3 distanceV = player->GetTrans() - GetTrans();

		//一定の範囲内にいるか
		if (distanceV.GetLength() < IObject::GetScale().GetLength() * PLAYER_DISTANCE_EXTEND_)
		{
			return true;
		}
	}

	return false;
}

void Enemy::Punched(const CollisionInfo& info, IObject3D* nodeObj)
{
	//プレイヤーの攻撃との判定(コライダーがnullptrの場合のみ処理→狙った敵以外は処理させない)
	if (info.object_->GetObjName().find("hand") != std::string::npos &&
		info.collider_ == nullptr)
	{
		if (damageCoolTime_ > 0.0f)
		{
			return;
		}

		//ノックバック
		KnockBack(info);

		//hp減らす
		auto stateChangeF = [=]() { Dead(); };
		auto stateChangeF2 = [=]() { ChangeEnemyState(std::make_unique<EnemyStateDamagedBegin>()); };
		Damaged(1, stateChangeF, stateChangeF2);

		//メッシュの波
		BeginDamagedWave(info, 1.4f);

		//パーティクル
		DamageParticle(60, 1, 1.0f, &info);

		//ノードの角度を加算するため
		SetAllNodeAddRots(*nodeObj);
	}
}

void Enemy::DecrementCoolTime()
{
	coolTime_ = max(coolTime_ - GameVelocityManager::GetInstance().GetVelocity(), 0);
}

//----------------------------------------------------------------
void Enemy::Update()
{
	//ダメージ受けるクールタイムもゲームスピードをかける
	damageCoolTime_ -= 1.0f * GameVelocityManager::GetInstance().GetVelocity();

	//アニメーションもゲームスピード
	SetAnimationSpeed(min(GameVelocityManager::GetInstance().GetVelocity() * 3.0f,
		GameVelocityManager::GetInstance().GAME_VELOCITY_MAX_) * animeSpeedExtend_);

	//演出更新
	ObjectFBX::EffectUpdate();

	//ノードごとの当たり判定
	UpdateNodeColliders();

	//メッシュ波
	waves_.Update(GameVelocityManager::GetInstance().GetVelocity());

	//ステート
	if (state_)
	{
		state_->Update();
	}
	//構えステート
	if (stanceState_)
	{
		stanceState_->Update();
	}

	//親クラスの処理
	Character::Update();
}

void Enemy::DrawShadow()
{
	ObjectFBX::DrawModel(nullptr, nullptr, ObjectFBX::PipelineStateNum::SHADOW_HULL_DOMAIN);
}

void Enemy::Draw()
{
	ObjectFBX::DrawModel(nullptr, nullptr, ObjectFBX::PipelineStateNum::HULL_DOMAIN);

	//ノードごとの当たり判定描画
	DrawNodeColliders();
}


//----------------------------------------------------------------------------------
void Enemy::KnockBack(const CollisionInfo& info)
{
	//距離のベクトル
	Vec3 distanceVec = -info.object_->GetVelocity();
	distanceVec.y = 0;

	//ダメージを受けるクールタイム
	damageCoolTime_ = 20;

	//構え解除
	ChangeEnemyStanceState(std::make_unique<EnemyStateAttackStanceEnd>());

	//武器持っていたら落とす
	if (weapon_)
	{
		distanceVec.Normalized();
		distanceVec.y += 0.4f;
		FallWeapon(Vec3(distanceVec.x, distanceVec.y, distanceVec.z) * WEAPON_FALL_VEL_EXTEND_);
	}
}

//------------------------------------------------------------------------------------------------------------------------
void Enemy::DamageParticle(int32_t particleNum, uint64_t interval, float vecPow, const CollisionInfo* info, IObject3D* obj, Vec3* pos, const Vec3& offsetPosExtend)
{
	//間隔的に出すため
	if (interval != 0 && GetCount() % interval != 0)
	{
		return;
	}

	for (int32_t i = 0; i < particleNum; ++i)
	{
		float scaleTmp = IObject::GetScale().GetLength();

		Vec3 posL = { 0,0,0 };
		//衝突相手のポインタセットされてたら
		if (info)
		{
			posL = { info->inter_.m128_f32[0],info->inter_.m128_f32[1],info->inter_.m128_f32[2] };
		}
		//ポジション指定されていたら
		if (pos)
		{
			posL = *pos;
		}
		//引数がセットされてたらその座標をそのまま使う
		else if (obj)
		{
			posL = obj->GetWorldTrans();
		}
		Vec3 addPos = Vec3(GetRand(-IObject::GetScale().x, IObject::GetScale().x) * offsetPosExtend.x,
			GetRand(-IObject::GetScale().y, IObject::GetScale().y) * offsetPosExtend.y,
			GetRand(-IObject::GetScale().z, IObject::GetScale().z) * offsetPosExtend.z);

		posL += addPos;

		const int32_t LIFE_TIME = 20;

		//相手の速度も使う
		Vec3 infoVec = { 0,0,0 };
		if (info)
		{
			infoVec = info->object_->GetVelocity().GetNormalized();
		}

		const float ADD_VEC = IObject::GetScale().y / 16.0f;

		Vec3 vel = Vec3(infoVec.x * GetRand(-0.1f, 1.0f) + GetRand(-ADD_VEC, ADD_VEC) * vecPow,
			infoVec.y * GetRand(-0.1f, 1.0f) + GetRand(-ADD_VEC, ADD_VEC) * vecPow,
			infoVec.z * GetRand(-0.1f, 1.0f) + GetRand(-ADD_VEC, ADD_VEC) * vecPow);

		float scale = scaleTmp / 30.0f;
		float scale2 = 0;

		ParticleManager::GetInstance()->Add(LIFE_TIME, posL, vel, { 0,-0.0002f,0 }, scale, scale2, { 3.0f,0.02f,0.02f,0.95f }, { 3.0f,0.02f,0.02f,0.95f },
			ParticleManager::BLEND_NUM::CRYSTAL,
			PI * 4.0f, -PI * 4.0f);
	}
}

void Enemy::DeadNodesParticle(int32_t particleNum, uint64_t interval)
{
	for (auto& node : GetNodeColliders())
	{
		//ノードのワールド座標（親オブジェクトの行列も反映）
		Vec3 posL = node->GetLocalTrans();

		DamageParticle(particleNum, interval, 0.3f, nullptr, nullptr, &posL);
	}
}

//---------------------------------------------------------
void Enemy::OnCollision(const CollisionInfo& info)
{
	OnCollision(this, info);
}

void Enemy::OnCollision(IObject3D* obj, const CollisionInfo& info)
{
	//殴られたら
	Punched(info, obj);

	//弾に当たったら
	if (info.object_->GetObjName() == "bullet")
	{
		//弾を撃った本人だったら
		if (this == dynamic_cast<Bullet*>(info.object_)->GetOwner())
		{
			return;
		}

		//ノックバック
		KnockBack(info);

		//今のhp分ダメージ受けて倒れる
		auto stateChangeF = [=]() { Dead(); };
		Damaged(hp_, stateChangeF);

		//ノードの角度を加算するため
		SetAllNodeAddRots(*obj, 1.5f);

		//メッシュの波
		BeginDamagedWave(info, 0.9f);

		//パーティクル
		DamageParticle(100, 1, 1.3f, &info, obj, nullptr);
	}
	//銃に当たったら
	else if (info.object_->GetObjName() == "gun")
	{
		Gun* gun = dynamic_cast<Gun*>(info.object_);

		if (!(gun != nullptr && gun->GetParent() == nullptr))
		{
			return;
		}

		//ノックバック
		KnockBack(info);

		//投げられているときのみ
		if (gun->GetIsThrowing() && gun->GetFallVelocity().GetLength() != 0)
		{
			//ノードの角度を加算するため
			SetAllNodeAddRots(*obj, 0.7f);
			//ダメージステートにする
			auto stateChangeF = [=]() { Dead(); };
			auto stateChangeF2 = [=]() { ChangeEnemyState(std::make_unique<EnemyStateDamagedBegin>()); };
			Damaged(0, stateChangeF, stateChangeF2);

			//メッシュの波
			BeginDamagedWave(info, 0.2f);
		}
		else
		{
			//武器拾う
			Vec3 lPos = { 0,0,0 };
			gun->SetRot(WEAPON_ROT_);
			PickUpWeapon(gun, &lPos);
			//武器の親ノード設定
			gun->ParentFbxNode(this, model_, WEAPON_PARENT_NODE_NAME_);

			state_->ChangeState();
		}
	}
	//敵同士で当たったらめり込まないようにする
	else if (info.object_->GetObjName().find("enemy") != std::string::npos)
	{
		//長さ
		float length = (info.object_->GetScale().x + obj->GetWorldScale().x);
		//距離のベクトル
		Vec3 distanceVec = obj->GetWorldTrans() - info.object_->GetTrans();
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
	//素手攻撃中にプレイヤーに当たったら
	else if (GetAnimData(AnimationNum::PUNCH).isPlay_ && info.object_->GetObjName() == "player")
	{
		//キャラクターのvirtual関数で死亡処理
		auto player = dynamic_cast<Character*>(info.object_);
		if (!player->GetIsDead())
		{
			CollisionInfo linfo = CollisionInfo(this, GetCollider(), info.inter_);
			player->Punched(linfo);
		}
	}
}