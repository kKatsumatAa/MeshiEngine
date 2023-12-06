#include "Player.h"
#include "KeyboardInput.h"
#include "DebugText.h"
#include "CameraManager.h"
#include "ParticleManager.h"
#include "PadInput.h"
#include "MouseInput.h"
#include "GameVelocityManager.h"
#include "PlayerAttackState.h"
#include "PlayerState.h"
#include "Bullet.h"
#include "PlayerUI.h"
#include "PlayerUIState.h"
#include "PostEffectManager.h"
#include "ObjectManager.h"
#include "LevelManager.h"


using namespace DirectX;


Player::~Player()
{
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
		assert(0);
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
	deadTimerMax_ = DEAD_TIMER_MAX_;

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

	return true;
}

void Player::ChangePlayerState(std::unique_ptr<PlayerState> state)
{
	state_.reset();
	state_ = std::move(state);
	state_->SetPlayer(this);
	state_->Initialize();
}

void Player::DirectionUpdate()
{
	//マウスの動きでカメラ角度変更
	mouseVel_ += MouseInput::GetInstance().GetCursorVelocity() * MOUSE_VELOCITY_TMP_;
	mouseVel_ *= MOUSE_GAME_VEL_ATTEN_;

	//回転
	Vec3 rotMove = {
		-mouseVel_.y ,
		mouseVel_.x,
		0
	};
	//角度を足す
	cameraRot_ = (cameraRot_ + rotMove);
	cameraRot_.x = (min(cameraRot_.x, PI / 2.0f * 0.9f));
	cameraRot_.x = (max(cameraRot_.x, -PI / 2.0f * 0.9f));

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
	if (CheckRayOfEyeHit(GetFrontVec(), 10000, colAttr, &info))
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

	//カメラ取得（借りてるだけ）
	Camera* camera = CameraManager::GetInstance().GetCamera("playerCamera");

	Vec3 velocity = { 0,0,0 };
	//向いてる方向に移動
	if (input->KeyPush(DIK_UPARROW) || input->KeyPush(DIK_W) || PadInput::GetInstance().GetLeftStickTilt().y < 0)
	{
		velocity += { frontVec_.x, 0, frontVec_.z };
	}
	if (input->KeyPush(DIK_DOWNARROW) || input->KeyPush(DIK_S) || PadInput::GetInstance().GetLeftStickTilt().y > 0)
	{
		velocity += { -frontVec_.x, 0, -frontVec_.z };
	}
	if (input->KeyPush(DIK_LEFTARROW) || input->KeyPush(DIK_A) || PadInput::GetInstance().GetLeftStickTilt().y < 0)
	{
		velocity += { -rightVec_.x, 0, -rightVec_.z };
	}
	if (input->KeyPush(DIK_RIGHTARROW) || input->KeyPush(DIK_D) || PadInput::GetInstance().GetLeftStickTilt().y > 0)
	{
		velocity += { rightVec_.x, 0, rightVec_.z };
	}

	//ゲームスピードを移動で足す(ジャンプ中でスペース押してなければ)
	if (!(!isOnGround_ && KeyboardInput::GetInstance().KeyPush(DIK_SPACE)))
	{
		GameVelocityManager::GetInstance().AddGameVelocity(velocity.GetLength() * MOVE_ADD_VEL_EXTEND_);
	}

	//ジャンプ中でスペース押しっぱなしだったら
	if (!isOnGround_ && KeyboardInput::GetInstance().KeyPush(DIK_SPACE) && velocity.GetLength())
	{
		GameVelocityManager::GetInstance().AddGameVelocity(-0.001f);
	}

	//位置セット(ゲームスピードをかける)
	SetTrans(GetTrans() + velocity * VELOCITY_TMP_ * GameVelocityManager::GetInstance().GetVelocity());

	//地面との判定
	std::function<void()>gameSpeedAddFunc = [=]() {GameVelocityManager::GetInstance().AddGameVelocity(1.0f); };
	OnGroundAndWallUpdate(HEIGHT_FROM_GROUND_, GameVelocityManager::GetInstance().GetVelocity(), KeyboardInput::GetInstance().KeyPush(DIK_SPACE), gameSpeedAddFunc);

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
	//クリックか外部で左クリック処理したいときにフラグ立てる
	isClickLeft_ = (MouseInput::GetInstance().GetTriggerClick(CLICK_LEFT) || isClickLeft_);

	//素手や銃などのステート
	state_->Update();

	//uiの位置
	UpdateUI();

	Character::Update();

	//オフ
	isClickLeft_ = false;
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
	//海用の
	PostEffectManager::GetInstance().GetPostEffect1()->effectFlags_.seaDirRot = { 0,0,0 };

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
	if (CheckRayOfEyeHit(GetFrontVec(), 10000, COLLISION_ATTR_ALL ^ COLLISION_ATTR_ALLIES, &info))
	{
		targetDir = Vec3(info.inter.m128_f32[0], info.inter.m128_f32[1], info.inter.m128_f32[2]) - GetTrans();

		if (info.object->GetObjName().find("enemy") != std::string::npos)
		{
			isTargetEnemy = true;
		}
	}
	scale = min(Lerp(0.2f, 1.5f, max(1.0f - (targetDir.GetLength() / (GetScale().GetLength() * 35.0f)), 0)) * scale, 1.5f);
	if (isTargetEnemy)
	{
		scale = max(scale, 0.8f);
	}
	PlayerUI::GetInstance().SetScale2(scale);
}

void Player::ThrowWeapon()
{
	//武器投げる
	FallWeapon(GetFrontTargetVec(COLLISION_ATTR_ENEMYS).GetNormalized() * FALL_VEL_POW_ + Vec3(0, 0.2f, 0),
		nullptr, false);

	//ゲームスピード加算
	GameVelocityManager::GetInstance().AddGameVelocity(0.9f);

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
		////長さ
		float length = (info.object_->GetWorldScale().GetLength() + GetScale().GetLength());
		//距離のベクトル
		Vec3 infoPos = info.object_->GetWorldTrans();
		Vec3 distanceVec = IObject3D::GetWorldTrans() - infoPos;
		//仮でyは動かさない
		distanceVec.y = 0;
		distanceVec.Normalized();

		//めり込まないように位置セット(半径＋半径の長さをベクトルの方向を使って足す)
		Vec3 ansPosE = infoPos + distanceVec * length * 1.00f;
		IObject3D::SetTrans(ansPosE);
	}
}
