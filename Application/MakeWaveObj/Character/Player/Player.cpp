#include "Player.h"
#include "KeyboardInput.h"
#include "DebugText.h"
#include "CameraManager.h"
#include "ParticleManager.h"
#include "PadInput.h"
#include "MouseInput.h"
#include "GameVelocityManager.h"
#include "PlayerHandState.h"
#include "PlayerState.h"
#include "Bullet.h"
#include "PlayerUI.h"
#include "PlayerUIState.h"
#include "PostEffectManager.h"
#include "ObjectManager.h"
#include "LevelManager.h"
#include "Replay.h"
#include "PlayerReplayState.h"
#include "StageManager.h"
#include "StageState.h"

using namespace DirectX;


Player::~Player()
{
	//死んだら武器落とす
	//（unique_ptrでobjectManagerで管理してるので、worldMat_のparent_がおかしなことになるため）
	if (weapon_)
	{
		//落とすときの角度
		weapon_->SetRot({ 0,PI,0 });
	}
	FallHaveWeapon(DEAD_WEAPON_FALL_VEC_, nullptr, false);
}

std::unique_ptr<Player> Player::Create(std::unique_ptr<WorldMat> worldMat, Weapon* weapon)
{
	std::unique_ptr<Player> instance = std::make_unique<Player>();
	if (instance.get() == nullptr)
	{
		return nullptr;
	}

	//初期化
	if (!instance->Initialize(std::move(worldMat), weapon))
	{
		assert(false);
	}

	return std::move(instance);
}

bool Player::Initialize(std::unique_ptr<WorldMat> worldMat, Weapon* weapon)
{
	if (!ObjectFBX::Initialize(std::move(worldMat)))
	{
		return false;
	}

	//カメラの位置と合わせる
	SetTrans(CameraManager::GetInstance().GetCamera("playerCamera")->GetEye());
	//スケール
	SetScale({ SCALE_EXTEND_, SCALE_EXTEND_ ,SCALE_EXTEND_ });

	//手
	handManager_.reset();
	handManager_ = std::make_unique<PlayerHandManager>();
	handManager_->Initialize(this);

	//hp
	hp_ = HP_TMP_;

	//回転用の角度の基
	cameraRot_ = GetRot();

	//死亡時のタイマー上限
	deadEffectTimerMax_ = DEAD_TIMER_MAX_;

	shotPosExtend_ = SHOT_POS_EXTEND_;

	if (weapon)
	{
		weapon_ = weapon;
		//武器の諸々設定
		PickUpWeapon(weapon_);
	}

	//武器持ってたらステートかえる
	if (weapon_ == nullptr)
	{
		ChangePlayerState(std::make_unique<PlayerStateBareHands>());
	}
	else
	{
		ChangePlayerState(std::make_unique<PlayerStateHaveWeapon>());
	}

	//リプレイ
	InitializeReplayState();

	return true;
}


//---------------------------------------------------------------------------------
void Player::ChangePlayerReplayState(std::unique_ptr<PlayerReplayState> state)
{
	replayState_.reset();
	replayState_ = std::move(state);
	replayState_->SetPlayer(this);
}

void Player::InitializeReplayState()
{
	//リプレイのステート
	ChangePlayerReplayState(std::make_unique<PlayerReplayStateSavingData>());
	replayState_->Initialize();

	//リプレイのステートの更新処理をセット
	playerState_->SetCommonUpdateF(replayStateF_);
}

void Player::ChangeToReplayingState()
{
	ChangePlayerReplayState(std::make_unique<PlayerReplayStateReplaying>());

	replayState_->Initialize();

	//リプレイのステートの更新処理をセット
	playerState_->SetCommonUpdateF(replayStateF_);
}

void Player::ChangePlayerState(std::unique_ptr<PlayerState> state)
{
	playerState_.reset();
	playerState_ = std::move(state);
	playerState_->SetPlayer(this);
	playerState_->Initialize();

	//リプレイのステートの更新処理をセット
	playerState_->SetCommonUpdateF(replayStateF_);
}

//--------------------------------------------------------------------------------

void Player::DirectionUpdate()
{
	//実際の計算
	DirectionUpdateCalcPart(MouseInput::GetInstance().GetCursorVelocity());

	//リプレイのデータ保存
	StageManager::GetInstance().GetReplay()->SetMouseCursorVel(MouseInput::GetInstance().GetCursorVelocity());
}

void Player::DirectionUpdateCalcPart(const Vec2& mouseVec)
{
	//マウスの動きでカメラ角度変更
	mouseVel_ += mouseVec * MOUSE_VELOCITY_TMP_;
	mouseVel_ *= MOUSE_GAME_VEL_ATTEN_;

	//回転
	Vec3 rotMove = {
		-mouseVel_.y ,
		mouseVel_.x,
		0
	};
	//角度を足す
	cameraRot_ = (cameraRot_ + rotMove);
	cameraRot_.x = (min(cameraRot_.x, CAMERA_ROT_RIMIT_));
	cameraRot_.x = (max(cameraRot_.x, -CAMERA_ROT_RIMIT_));

	//正面ベクトルを回転
	RotateFrontVec(cameraRot_);

	//角度
	SetRot(cameraRot_);

	//カメラの右方向ベクトルを出す
	rightVec_ = upVec_.Cross(frontVec_);
}

void Player::RotateFrontVec(const Vec3& rot)
{
	frontVec_ = GetFrontVecTmp();
	//回転
	frontVec_ = GetTurnVec3UseQuaternionAndRot(frontVec_, rot);

	//正面ベクトルセット
	SetFrontVec(frontVec_);

	//海用の
	PostEffectManager::GetInstance().GetPostEffect1()->effectFlags_.seaDirRot = rot;
}

void Player::UpdateUseCameraTarget()
{
	auto camera = CameraManager::GetInstance().GetCamera("playerCamera");
	//カメラの注視点に回転したベクトルセット
	camera->SetTarget(GetTrans() + frontVec_);
}

Vec3 Player::GetWeaponPosTmp()
{
	return {
		GetScale().x * WEAPON_POS_EXTEND_.x ,
					GetScale().y * WEAPON_POS_EXTEND_.y ,
					GetScale().z * WEAPON_POS_EXTEND_.z
	};
}

Vec3 Player::GetFrontTargetVec(uint16_t colAttr)
{
	Vec3 ansVec = { 0,0,0 };

	RaycastHit info;
	if (CheckRayOfEyeHit(GetFrontVec(), EYE_RAY_LENGTH_, colAttr, &info))
	{
		ansVec = Vec3(info.inter.m128_f32[0], info.inter.m128_f32[1], info.inter.m128_f32[2]) - (
			GetWeapon()->GetWorldTrans());
	}

	return ansVec;
}

void Player::Move()
{
	//キー入力
	KeyboardInput* input = &KeyboardInput::GetInstance();

	bool leftKey = input->KeyPush(DIK_LEFTARROW) || input->KeyPush(DIK_A);
	bool rightKey = input->KeyPush(DIK_RIGHTARROW) || input->KeyPush(DIK_D);
	bool upKey = input->KeyPush(DIK_UPARROW) || input->KeyPush(DIK_W);
	bool downKey = input->KeyPush(DIK_DOWNARROW) || input->KeyPush(DIK_S);
	bool spaceKey = input->KeyPush(DIK_SPACE);

	//実際に計算など
	MoveCalcPart(leftKey, rightKey, upKey, downKey, spaceKey, GameVelocityManager::GetInstance().GetVelocity());

	//リプレイのデータ保存
	StageManager::GetInstance().GetReplay()->SetLeftKey(leftKey);
	StageManager::GetInstance().GetReplay()->SetRightKey(rightKey);
	StageManager::GetInstance().GetReplay()->SetUpKey(upKey);
	StageManager::GetInstance().GetReplay()->SetDownKey(downKey);
	StageManager::GetInstance().GetReplay()->SetSpaceKey(spaceKey);
}

void Player::MoveCalcPart(bool leftKey, bool rightKey, bool upKey, bool downKey, bool spaceKey, float moveVelRatio)
{
	//カメラ取得（借りてるだけ）
	Camera* camera = CameraManager::GetInstance().GetCamera("playerCamera");

	Vec3 velocity = { 0,0,0 };
	//向いてる方向に移動
	if (upKey)
	{
		velocity += { frontVec_.x, 0, frontVec_.z };
	}
	if (downKey)
	{
		velocity += { -frontVec_.x, 0, -frontVec_.z };
	}
	if (leftKey)
	{
		velocity += { -rightVec_.x, 0, -rightVec_.z };
	}
	if (rightKey)
	{
		velocity += { rightVec_.x, 0, rightVec_.z };
	}

	//ゲームスピードを移動で足す(ジャンプ中でスペース押してなければ)
	if (!(!isOnGround_ && spaceKey))
	{
		GameVelocityManager::GetInstance().AddGameVelocity(velocity.GetLength() * MOVE_ADD_VEL_EXTEND_);
	}

	//ジャンプ中でスペース押しっぱなしだったら
	if (!isOnGround_ && spaceKey && velocity.GetLength())
	{
		GameVelocityManager::GetInstance().AddGameVelocity(JUMPING_ADD_GAME_VEL_);
	}

	//位置セット(ゲームスピードをかける)
	SetTrans(GetTrans() + velocity * VELOCITY_TMP_ * moveVelRatio);

	//地面との判定
	std::function<void()>gameSpeedAddFunc = [=]() {GameVelocityManager::GetInstance().AddGameVelocity(GameVelocityManager::GetInstance().GAME_VELOCITY_MAX_); };
	OnGroundAndWallUpdate(HEIGHT_FROM_GROUND_, moveVelRatio, spaceKey, gameSpeedAddFunc);

	//コライダー更新
	ObjectFBX::WorldMatColliderUpdate();

	//カメラをプレイヤーと同じ位置に
	camera->SetEye(GetTrans());

	//カメラの注視点に回転したベクトルセット
	UpdateUseCameraTarget();

	//一定以上落ちたら
	if (GetTrans().y < GetScale().y * DEAD_MIN_POS_EXTEND_)
	{
		DeadLava();
	}
}


void Player::Update()
{
	//マウスでアクションのフラグをセット
	UpdatePlayerActionFromMouse();

	//uiの位置
	UpdateUI();

	Character::Update();

	//素手や銃などのステート
	playerState_->Update();
}

void Player::Draw()
{
	if (isDead_)
	{
		return;
	}

	//手の描画
	handManager_->Draw();
}

void Player::Dead()
{
	//hpが0になったら
	isDead_ = true;

	//手を削除
	handManager_->DeleteHands();

	//ステートを変更
	if (isPunched_)
	{
		ChangePlayerState(std::make_unique<PlayerStateDeadEffectPunched>());
	}
	else
	{
		//海用の
		PostEffectManager::GetInstance().GetPostEffect1()->effectFlags_.seaDirRot = { 0,0,0 };
		ChangePlayerState(std::make_unique<PlayerStateDeadEffect>());
	}
}

void Player::UpdateLavaDead()
{
	auto landShapes = ObjectManager::GetInstance().GetObjs(
		LevelManager::S_OBJ_GROUP_NAME_, COLLISION_ATTR_LANDSHAPE);

	if (landShapes.size() <= 0)
	{
		DeadLava();
	}
}

void Player::DeadLava()
{
	if (!isDead_)
	{
		isDead_ = true;
		//手を削除
		handManager_->DeleteHands();
		//溶岩で死んだときの演出
		ChangePlayerState(std::make_unique<PlayerStateDeadEffect2>());
	}
}

void Player::Punched(const CollisionInfo& info, IObject3D* nodeObj)
{
	if (nodeObj == nullptr)
	{
		SetPosOfEnemyAttack(info.object_->GetWorldTrans());
		SetIsPunched(true);
		Damaged(HP_TMP_, [=]() {Dead(); });
	}
}

void Player::UpdateUI()
{
	Vec3 targetDir = GetFrontVec();
	float scale = 1.0f;
	bool isTargetEnemy = false;

	//レイを飛ばしてターゲットまでの距離を計算、uiの大きさを変える
	RaycastHit info;
	if (CheckRayOfEyeHit(GetFrontVec(), EYE_RAY_LENGTH_, COLLISION_ATTR_ALL ^ COLLISION_ATTR_ALLIES, &info))
	{
		targetDir = Vec3(info.inter.m128_f32[0], info.inter.m128_f32[1], info.inter.m128_f32[2]) - GetTrans();

		if (info.object->GetObjName().find("enemy") != std::string::npos)
		{
			isTargetEnemy = true;
		}
	}

	//距離が遠いほどレティクルが小さく
	scale = min(Lerp(RETICLE_SIZE_MIN_, RETICLE_SIZE_MAX_,
		max(RETICLE_DIST_SCALE_RATIO_MAX_ - (targetDir.GetLength() / (GetScale().GetLength() * RETICLE_LENGTH_MAX_)), 0)) * scale,
		RETICLE_SIZE_MAX_);

	//レイの先が敵だったら小さくなりすぎないように
	if (isTargetEnemy)
	{
		scale = max(scale, RAY_HIT_ENEMY_RETICLE_SIZE_MIN_);
	}
	PlayerUI::GetInstance().SetScaleRatio(scale);
}

void Player::UpdatePlayerActionFromMouse()
{
	//クリックか外部で左クリック処理したいときにフラグ立てる
	isClickLeft_ = (MouseInput::GetInstance().GetTriggerClick(CLICK_LEFT));
	isClickRight_ = (MouseInput::GetInstance().GetTriggerClick(CLICK_RIGHT));

	//リプレイのデータ
	StageManager::GetInstance().GetReplay()->SetIsLeftClickTrigger(isClickLeft_);
	StageManager::GetInstance().GetReplay()->SetIsRightClickTrigger(isClickRight_);
}

void Player::ThrowWeapon()
{
	//武器投げる
	FallHaveWeapon(GetFrontTargetVec(COLLISION_ATTR_ENEMYS).GetNormalized() * FALL_VEL_POW_ + THROW_WEAPON_VEC_,
		nullptr, false);

	//ゲームスピード加算
	GameVelocityManager::GetInstance().AddGameVelocity(THROW_WEAPON_ADD_GAME_VEL_);

	//ステート変更(素手)
	ChangePlayerState(std::make_unique<PlayerStateBareHands>());
}

void Player::OnCollision(const CollisionInfo& info)
{
	//弾に当たったらダメージ
	if (info.object_->GetObjName() == "bullet")
	{
		Bullet* bullet = dynamic_cast<Bullet*>(info.object_);
		//演出用に弾撃った敵の位置保存
		posOfEnemyAttack_ = bullet->GetOwnerPos();
		//ダメージ
		Damaged(hp_, [=]() { Dead(); });
	}
	//敵に当たったら
	else if (info.collider_->GetAttribute() & COLLISION_ATTR_ENEMYS &&
		!isDead_)
	{
		//長さ
		float length = (info.object_->GetWorldScale().GetLength() + GetScale().GetLength()) / 2.0f;
		//距離のベクトル
		Vec3 infoPos = info.object_->GetWorldTrans();
		Vec3 distanceVec = IObject3D::GetWorldTrans() - infoPos;
		//仮でyは動かさない
		distanceVec.y = 0;
		distanceVec.Normalized();

		//めり込まないように位置セット(半径＋半径の長さをベクトルの方向を使って足す)
		Vec3 ansPosE = infoPos + distanceVec * length;
		IObject3D::SetTrans(ansPosE);
	}
}
