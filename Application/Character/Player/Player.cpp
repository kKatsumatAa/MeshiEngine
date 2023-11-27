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

	frontVec_ = GetFrontVecTmp();
	//回転
	frontVec_ = GetTurnVec3UseQuaternionAndRot(frontVec_, cameraRot_);

	//正面ベクトルセット
	SetFrontVec(frontVec_);

	//海用の
	PostEffectManager::GetInstance().GetPostEffect1()->effectFlags_.seaDirRot = cameraRot_;

	//角度
	SetRot(cameraRot_);


	//カメラの右方向ベクトルを出す
	rightVec_ = upVec_.Cross(frontVec_);
}

Vec3 Player::GetWeaponPosTmp()
{
	return {
		GetScale().x * WEAPON_POS_EXTEND_.x ,
					GetScale().y * WEAPON_POS_EXTEND_.y ,
					GetScale().z * WEAPON_POS_EXTEND_.z
	};
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
	OnGroundAndWallUpdate(4.0f, GameVelocityManager::GetInstance().GetVelocity(), KeyboardInput::GetInstance().KeyPush(DIK_SPACE), gameSpeedAddFunc);

	//コライダー更新
	ObjectFBX::WorldMatColliderUpdate();

	//カメラをプレイヤーと同じ位置に
	camera->SetEye(GetTrans());

	//カメラの注視点に回転したベクトルセット
	camera->SetTarget(GetTrans() + frontVec_);

	//一定の位置に行ったら死亡
	if (GetTrans().y <= GetScale().y * DEAD_MIN_POS_EXTEND_)
	{
		isAlive_ = false;
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

void Player::Dead(const CollisionInfo& info)
{
	//hpが0になったら
	isDead_ = true;

	//手を削除
	handManager_->DeleteHands();

	Bullet* bullet = dynamic_cast<Bullet*>(info.object_);

	//演出用に弾撃った敵の位置保存
	bulletOwnerEnemyPos_ = bullet->GetOwnerPos();

	//ステートを変更
	ChangePlayerState(std::make_unique<PlayerStateDeadEffect>());
}

void Player::UpdateUI()
{
	Vec3 targetDir = GetFrontVec();
	float scale = 1.0f;
	//レイを飛ばしてターゲットまでの距離を計算、uiの大きさを変える
	RaycastHit info;
	if (CheckRayOfEyeHit(GetFrontVec(), 10000, COLLISION_ATTR_ALL ^ COLLISION_ATTR_ALLIES, &info))
	{
		targetDir = Vec3(info.inter.m128_f32[0], info.inter.m128_f32[1], info.inter.m128_f32[2]) - GetTrans();

		if (info.object->GetObjName().find("enemy") != std::string::npos)
		{
			scale = 2.0f;
		}
	}
	scale = min(Lerp(0.2f, 1.5f, max(1.0f - (targetDir.GetLength() / (GetScale().GetLength() * 35.0f)), 0)) * scale, 1.5f);
	PlayerUI::GetInstance().SetScale2(scale);
}

void Player::ThrowWeapon()
{
	FallWeapon(GetFrontVec() * FALL_VEL_POW_ + Vec3(0, 0.2f, 0));

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
		//ダメージ
		Damaged(hp_, [=]() { Dead(info); });
	}
}
