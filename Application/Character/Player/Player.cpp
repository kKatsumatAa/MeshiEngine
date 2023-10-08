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
	Vec2 vel = MouseInput::GetInstance().GetCursorVelocity() * MOUSE_VELOCITY_TMP_;

	//マウスの動きでゲームスピードを足す
	GameVelocityManager::GetInstance().AddGameVelocity(vel.GetLength() * MOUSE_GAME_VEL_EXTEND_, "mouse");

	//回転
	Vec3 rotMove = {
		-vel.y * ANGLE_VEL_EXTEND_,
		vel.x * ANGLE_VEL_EXTEND_,
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

	//角度
	SetRot(cameraRot_);


	//カメラの右方向ベクトルを出す
	rightVec_ = upVec_.Cross(frontVec_);
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
}

void Player::Update()
{
	//クリックか外部で左クリック処理したいときにフラグ立てる
	isClickLeft_ = (MouseInput::GetInstance().GetTriggerClick(CLICK_LEFT) || isClickLeft_);

	//素手や銃などのステート
	state_->Update();

	Character::Update();

	//手のアップデート
	handManager_->Update();

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
