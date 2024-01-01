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
#include "EnemyPart.h"

using namespace DirectX;

const float Enemy::S_LENGTH_MAX_ = 10000;


Enemy::~Enemy()
{
	InactiveEmergeLight();
}

void Enemy::EmergeInitialize()
{
	LightManager* lightM = LevelManager::GetInstance().GetLightManager();

	//使えるライトがあれば
	if (lightM->GetDoNotUsePointLightIndex(lightIndexTmp_))
	{
		lightM->SetPointLightActive(lightIndexTmp_, true);
		lightM->SetPointLightAtten(lightIndexTmp_, POINT_LIGHT_ATTEN_);
		lightM->SetPointLightColor(lightIndexTmp_, POINT_LIGHT_COLOR_);
		lightM->SetPointLightPos(lightIndexTmp_,
			{ IObject::GetTrans().x,IObject::GetTrans().y, IObject::GetTrans().z });
	}

	//ディゾルブ
	SetisDissolve(true);
	SetDissolveRatio(INIT_DISSOLVE_RATE_);
	//ディゾルブ画像
	uint64_t handle;
	handle = TextureManager::LoadGraph("dissolveMask.png");
	SetDissolveTexHandle(handle);

	//演出更新
	ObjectFBX::EffectUpdate();

	//fbx前提
	ModelFBX* modelFbx = dynamic_cast<ModelFBX*>(GetModel());
	//ノードごとの当たり判定
	InitializeNodeColliders(modelFbx, NODE_COLLIDER_SCALE_, COLLISION_ATTR_ENEMYS);
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
		assert(false);
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
		weapon_->SetScale(weapon_->GetScale() * WEAPON_SCALE_RATE_);
		//武器を諸々設定
		PickUpWeapon(weapon_);
	}

	//hpセット
	hp_ = HP_TMP_;
	oldHP_ = hp_;

	waveNum_ = waveNum;
	emergeCoolTime_ = (float)coolTime;

	//死亡時のタイマー上限
	deadEffectTimerMax_ = DEAD_TIMER_MAX_;

	//model
	model->SetMaterialExtend(MODEL_MATERIAL_RATE_);

	//色
	SetColor(COLOR_);

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
	CollisionWallAndFloor();

	WalkToTarget(targetPos, isWave);
}

void Enemy::Attack(const Vec3& targetPos)
{
	//武器で攻撃
	if (weapon_ != nullptr && weapon_->GetIsAlive())
	{
		Vec3 directionV = targetPos - weapon_->GetBoneWorldTrans();
		weapon_->Attack(directionV.GetNormalized(), CONSUM_BULLET_NUM_, this, PARTICLE_SIZE_EXTEND_);
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
	directionVec_.y = DIR_Y_TMP_;
	directionVec_.Normalized();

	//現在のスピードに方向ベクトル足し、ゲームスピードをかける
	velocity_ = GetVelocity() + directionVec_ * VELOCITY_RATE_ * GameVelocityManager::GetInstance().GetVelocity();
	//スピードの上限は超えないように
	float length = velocity_.GetLength();
	//スピードがプラスになってたら
	if (velocity_.Dot(directionVec_) >= END_VEL_OF_DAMAGE_COOL_)
	{
		directionVec_ = targetPos - IObject::GetTrans();
		//ある程度近づいたら止まる
		if (GetPlayerIsWithinRange())
		{
			velocity_ = INIT_VELOCITY_;
		}
		directionVec_.Normalized();

		//ダメージのクールリセット
		damageCoolTime_ = INIT_DAMAGE_COOL_TIME_;
	}
	//スピードの上限超えない
	velocity_ = velocity_.GetNormalized() * min(fabsf(length), GameVelocityManager::GetInstance().GetVelocity() * VELOCITY_RATE_);

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

	Vec3 addRot = INIT_NODE_ADD_ROT_;
	//Spine2(仮)からの距離で回転方向を決めるため(回転の逆行列をかけて元の位置関係に戻したうえで)
	Vec3 localPosFromSpine2 = nodeObj.GetLocalTrans() * GetWorldMat()->GetRotMat().GetInverseMatrix()
		- ObjectFBX::GetNodeColliderObj("Spine2")->GetLocalTrans() * GetWorldMat()->GetRotMat().GetInverseMatrix();

	addRot.x = max(min(-localPosFromSpine2.y, PI * NODE_ADD_ROT_RATE_), -PI * NODE_ADD_ROT_RATE_) * angleExtend;
	addRot.y = max(min(-localPosFromSpine2.x, PI * NODE_ADD_ROT_RATE_), -PI * NODE_ADD_ROT_RATE_) * angleExtend;

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

		Vec3 addRotLeg = INIT_NODE_ADD_ROT_;
		addRotLeg.x = max(min(localPosFromLeg.y, PI * LEG_ADD_ROT_RATE_), -PI * LEG_ADD_ROT_RATE_) * angleExtend;
		addRotLeg.y = max(min(-localPosFromLeg.x, PI * LEG_ADD_ROT_RATE_), -PI * LEG_ADD_ROT_RATE_) * angleExtend;

		damagedAddRots_.push_back({ leftOrRightStr + "Leg", INIT_NODE_ADD_ROT_, addRotLeg });
	}

	damagedAddRots_.push_back({ "Spine2", INIT_NODE_ADD_ROT_, addRot });
}

void Enemy::WalkWaveUpdate()
{
	float gameVel = GameVelocityManager::GetInstance().GetVelocity() * GetAnimeSpeedExtend();

	walkWaveTimer_ += gameVel;

	//間隔が来たらステージに波紋
	if ((int32_t)walkWaveTimer_ % (int32_t)WALK_MOVE_INTERVAL_ == 0
		&& walkWaveTimer_ - beforeWalkTime_ >= WALK_MOVE_INTERVAL_)
	{
		BeginWaveStage(GetWorldTrans() - Vec3(0, GetScale().y, 0), { GetScale().z * STAGE_WAVE_WIDTH_RATE_, GetScale().y * STAGE_WAVE_HEIGHT_RATE_ },
			GetScale().z * STAGE_WAVE_DISTANCE_RATE_, STAGE_WAVE_TIME_);
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
	Quaternion q = Quaternion::DirectionToDirection(fVTmp, pDVTmp, USE_QUATERNION_AXIS_);
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
	if (hp_ <= DEAD_HP_)
	{
		lDissolvePow = Lerp(DISSOLVE_POW_, IObject3D::DISSOLVE_RATE_MAX_, EaseOut(t));
	}
	//hpによってディゾルブ(減った後のhpとの線形補完)
	IObject3D::SetDissolveRatio((IObject3D::DISSOLVE_RATE_MAX_ - Lerp((float)oldHP_, (float)hp_, t) / (float)HP_TMP_) * lDissolvePow);

	//ポリゴンごとに法線方向動くように
	Mesh::PolygonOffset offsetData;
	offsetData.interval = GetRand(MESH_OFFSET_INTERVAL_MIN_, MESH_OFFSET_INTERVAL_MAX_)
		* (GameVelocityManager::GetInstance().GAME_VELOCITY_MAX_ - GameVelocityManager::GetInstance().GetVelocity());
	//長さや割合も
	offsetData.length = GetRand(-IObject::GetScale().x, IObject::GetScale().x) * MESH_OFFSET_LENGTH_RATE_;
	offsetData.ratio = (RATE_MAX_ - (float)hp_ / (float)HP_TMP_);
	ObjectFBX::SetMeshPolygonOffsetData(offsetData);
}

void Enemy::BeginDamagedWave(const CollisionInfo& info, float wavePow)
{
	Vec3 pos = { info.inter_.m128_f32[0] ,info.inter_.m128_f32[1] ,info.inter_.m128_f32[2] };

	//自分にメッシュの波
	for (int i = 0; i < MY_WAVE_NUM_; i++)
	{
		BeginWave(pos,
			{ GetScale().y * DAMAGE_MY_WAVE_WIDTH_RATE_ * wavePow, GetScale().GetLength() * DAMAGE_MY_WAVE_HEIGHT_RATE_ * wavePow },
			GetScale().GetLength() * DAMAGE_MY_WAVE_DISTANCE_RATE_,
			DAMAGE_MY_WAVE_TIME_ / wavePow * GetRand(DAMAGE_MY_WAVE_TIME_RATE_MIN_, DAMAGE_MY_WAVE_TIME_RATE_MAX_));
	}

	//ステージに波紋
	for (int i = 0; i < STAGE_WAVE_NUM_; i++)
	{
		BeginWaveStage(pos,
			{ GetScale().y * DAMAGE_STAGE_WAVE_WIDTH_RATE_ * wavePow,GetScale().GetLength() * DAMAGE_STAGE_WAVE_HEIGHT_RATE_ * wavePow },
			GetScale().GetLength() * DAMAGE_STAGE_WAVE_DISTANCE_RATE_,
			DAMAGE_STAGE_WAVE_TIME_ * GetRand(DAMAGE_STAGE_WAVE_TIME_RATE_MIN_, DAMAGE_STAGE_WAVE_TIME_RATE_MAX_));
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
		if (damageCoolTime_ > DAMAGE_COOL_TIME_END_)
		{
			return;
		}

		//部位(仮)
		DetachPart("RightHand", (GetTrans() - info.object_->GetTrans()).GetNormalized());

		//ノックバック
		KnockBack(info);

		//hp減らす
		auto stateChangeF = [=]() { Dead(); };
		auto stateChangeF2 = [=]() { ChangeEnemyState(std::make_unique<EnemyStateDamagedBegin>()); };
		Damaged(1, stateChangeF, stateChangeF2);

		//メッシュの波
		BeginDamagedWave(info, PUNCHED_WAVE_RATE_);

		//パーティクル
		DamageParticle(PUNCHED_PARTICLE_NUM_, PUNCHED_PARTICLE_INTERVAL_, PUNCHED_PARTICLE_VEC_POW_, &info);

		//ノードの角度を加算するため
		SetAllNodeAddRots(*nodeObj);
	}
}

void Enemy::DecrementEmergeCoolTime()
{
	emergeCoolTime_ = max(emergeCoolTime_ - GameVelocityManager::GetInstance().GetVelocity(), EMERGE_COOL_TIME_MIN_);
}

void Enemy::InactiveEmergeLight()
{
	LightManager* lightM = LevelManager::GetInstance().GetLightManager();
	if (GetLightIndexTmp() != GetLightIndexInit())
	{
		lightM->SetPointLightActive(GetLightIndexTmp(), false);
	}
}

void Enemy::SetEmergeLight(float rate)
{
	LightManager* lightM = LevelManager::GetInstance().GetLightManager();
	if (GetLightIndexTmp() != GetLightIndexInit())
	{
		Vec3 color = LerpVec3({ POINT_LIGHT_COLOR_.x,POINT_LIGHT_COLOR_.y,POINT_LIGHT_COLOR_.z },
			{ POINT_LIGHT_END_COLOR_.x,POINT_LIGHT_END_COLOR_.y,POINT_LIGHT_END_COLOR_.z }, rate);

		lightM->SetPointLightColor(GetLightIndexTmp(), { color.x,color.y ,color.z });
	}
}

void Enemy::DetachPart(const std::string& partName, const Vec3& dirVec)
{
	//メッシュからモデル生成
	auto partModel = ModelObj::CreateModelFromModelMesh(*model_, partName);

	if (partModel)
	{
		//オブジェクト生成
		auto enemyPart = EnemyPart::Create(*worldMat_.get(), partModel.get());
		//色
		enemyPart->SetColor(GetColor());

		//投げる
		Vec3 fallPos = enemyPart->GetTrans();
		FallWeapon(enemyPart.get(), dirVec, &fallPos);

		//オブジェクトを追加
		ObjectManager::GetInstance().AddObject(LevelManager::S_OBJ_GROUP_NAME_, std::move(enemyPart));
		//モデルも追加
		ModelManager::GetInstance().AddModelObj(std::move(partModel), partName);
	}
}

//----------------------------------------------------------------
void Enemy::Update()
{
	//ダメージ受けるクールタイムもゲームスピードをかける
	damageCoolTime_ -= GameVelocityManager::GetInstance().GetVelocity();

	//アニメーションもゲームスピード
	SetAnimationSpeed(min(GameVelocityManager::GetInstance().GetVelocity() * ANIMATION_SPEED_RATE_,
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
	distanceVec.y = OBJ_DIST_VEC_Y_;

	//ダメージを受けるクールタイム
	damageCoolTime_ = DAMAGE_COOL_TIME_TMP_;

	//構え解除
	ChangeEnemyStanceState(std::make_unique<EnemyStateAttackStanceEnd>());

	//武器持っていたら落とす
	if (weapon_)
	{
		distanceVec.Normalized();
		distanceVec.y += THROW_WEAPON_VEC_Y_;
		FallHaveWeapon(Vec3(distanceVec.x, distanceVec.y, distanceVec.z) * WEAPON_FALL_VEL_EXTEND_);
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

		const int16_t LIFE_TIME = PARTICLE_LIFE_TIME_;

		//相手の速度も使う
		Vec3 infoVec = { 0,0,0 };
		if (info)
		{
			infoVec = info->object_->GetVelocity().GetNormalized();
		}

		const float ADD_VEC = IObject::GetScale().y * PARTICLE_VEL_RATE_;

		Vec3 vel = Vec3(infoVec.x * GetRand(PARTICLE_VEL_RATE_MIN_, PARTICLE_VEL_RATE_MAX_) + GetRand(-ADD_VEC, ADD_VEC) * vecPow,
			infoVec.y * GetRand(PARTICLE_VEL_RATE_MIN_, PARTICLE_VEL_RATE_MAX_) + GetRand(-ADD_VEC, ADD_VEC) * vecPow,
			infoVec.z * GetRand(PARTICLE_VEL_RATE_MIN_, PARTICLE_VEL_RATE_MAX_) + GetRand(-ADD_VEC, ADD_VEC) * vecPow);

		float scale = scaleTmp * PARTICLE_SCALE_RATE_;
		float endScale = 0;

		//パーティクル生成
		ParticleManager::GetInstance()->Add(LIFE_TIME, posL, vel, PARTICLE_ACCEL_, scale, endScale,
			PARTICLE_START_COLOR_, PARTICLE_END_COLOR_,
			ParticleManager::BLEND_NUM::CRYSTAL,
			PARTICLE_START_ROT_, PARTICLE_END_ROT_);
	}
}

void Enemy::DeadNodesParticle(int32_t particleNum, uint64_t interval)
{
	for (auto& node : GetNodeColliders())
	{
		//ノードのワールド座標（親オブジェクトの行列も反映）
		Vec3 posL = node->GetLocalTrans();

		DamageParticle(particleNum, interval, DEAD_PARTICLE_VEL_RATE_, nullptr, nullptr, &posL);
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
		SetAllNodeAddRots(*obj, SHOOTED_NODE_ADD_ROT_RATE_);

		//メッシュの波
		BeginDamagedWave(info, SHOOTED_WAVE_RATE_);

		//パーティクル
		DamageParticle(SHOOTED_PARTICLE_NUM_, 1, SHOOTED_PARTICLE_VEL_RATE_, &info, obj, nullptr);
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
			SetAllNodeAddRots(*obj, HIT_WEAPON_NODE_ADD_ROT_RATE_);
			//ダメージステートにする
			auto stateChangeF = [=]() { Dead(); };
			auto stateChangeF2 = [=]() { ChangeEnemyState(std::make_unique<EnemyStateDamagedBegin>()); };
			Damaged(0, stateChangeF, stateChangeF2);

			//メッシュの波
			BeginDamagedWave(info, HIT_WEAPON_WAVE_RATE_);
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
		distanceVec.y = OBJ_DIST_VEC_Y_;
		distanceVec.Normalized();
		//位置セット(半径＋半径の長さをベクトルの方向を使って足す)
		IObject::SetTrans(info.object_->GetTrans() + distanceVec * length * PUSH_BACK_LENGTH_RATE_);

		//ｙは動かないようにする
		IObject::SetVelocity({ IObject::GetVelocity().x,0,IObject::GetVelocity().z });
		info.object_->SetVelocity({ info.object_->GetVelocity().x,0,info.object_->GetVelocity().z });
		//二つのベクトルの合計の長さ
		float addLength = GetVelocity().GetLength() + info.object_->GetVelocity().GetLength();
		//自分のスピードのベクトルの長さの割合（合計の長さで割る（0～1.0f））
		float myLengthRatio = GetVelocity().GetLength() / addLength;

		//衝突後の自分のスピードベクトルは[現在のスピードベクトル]+[相手から自分へのベクトル]*[相手の長さの割合]
		SetVelocity((GetVelocity() + distanceVec.GetNormalized() * addLength * (1.0f - myLengthRatio)) * PUSH_BACK_VEL_RATE_);
		//衝突後の相手のスピードベクトルは[現在のスピードベクトル]+[このインスタンスから相手へのベクトル]*[このインスタンスの長さの割合]
		info.object_->SetVelocity((info.object_->GetVelocity() - distanceVec.GetNormalized() * addLength * (myLengthRatio)) * PUSH_BACK_VEL_RATE_);
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