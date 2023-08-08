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
	if (!Object::Initialize(std::move(worldMat)))
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
	//カメラ取得（借りてるだけ）
	Camera* camera = CameraManager::GetInstance().GetCamera("playerCamera");

	//マウスの動きでカメラ角度変更
	Vec2 vel = MouseInput::GetInstance().GetCursorVelocity() * MOUSE_VELOCITY_TMP_;

	//マウスの動きでゲームスピードを足す
	GameVelocityManager::GetInstance().AddGameVelocity(vel.GetLength() * MOUSE_GAME_VEL_EXTEND_, "mouse");

	//回転
	Vec3 rotMove = {
		vel.y_ * ANGLE_VEL_EXTEND_,
		vel.x_ * ANGLE_VEL_EXTEND_,
		0
	};
	//角度を足す
	cameraRot_ = (cameraRot_ + rotMove);
	cameraRot_.x_ = (min(cameraRot_.x_, PI / 2.0f * 0.9f));
	cameraRot_.x_ = (max(cameraRot_.x_, -PI / 2.0f * 0.9f));

	frontVec_ = GetFrontVecTmp();
	//回転
	frontVec_ = GetTurnVec3UseQuaternionAndRot(frontVec_, cameraRot_);

	//正面ベクトルセット
	SetFrontVec(frontVec_);

	//角度
	SetRot(cameraRot_);


	//カメラの右方向ベクトルを出す
	rightVec_ = upVec_.Cross(frontVec_);
	//上方向も正しいものを
	//upVec = frontVec_.Cross(rightVec);

	//カメラの上方向セット
	//camera->SetUp(upVec);
}

void Player::Move()
{
	//キー入力
	KeyboardInput* input = &KeyboardInput::GetInstance();

	//カメラ取得（借りてるだけ）
	Camera* camera = CameraManager::GetInstance().GetCamera("playerCamera");

	Vec3 velocity_ = { 0,0,0 };
	//向いてる方向に移動
	if (input->KeyPush(DIK_UPARROW) || input->KeyPush(DIK_W) || PadInput::GetInstance().GetLeftStickTilt().y_ < 0)
	{
		velocity_ += { frontVec_.x_, 0, frontVec_.z_ };
	}
	if (input->KeyPush(DIK_DOWNARROW) || input->KeyPush(DIK_S) || PadInput::GetInstance().GetLeftStickTilt().y_ > 0)
	{
		velocity_ += { -frontVec_.x_, 0, -frontVec_.z_ };
	}
	if (input->KeyPush(DIK_LEFTARROW) || input->KeyPush(DIK_A) || PadInput::GetInstance().GetLeftStickTilt().y_ < 0)
	{
		velocity_ += { -rightVec_.x_, 0, -rightVec_.z_ };
	}
	if (input->KeyPush(DIK_RIGHTARROW) || input->KeyPush(DIK_D) || PadInput::GetInstance().GetLeftStickTilt().y_ > 0)
	{
		velocity_ += { rightVec_.x_, 0, rightVec_.z_ };
	}

	//ゲームスピードを移動で足す(ジャンプ中でスペース押してなければ)
	if (!(!isOnGround_ && KeyboardInput::GetInstance().KeyPush(DIK_SPACE)))
	{
		GameVelocityManager::GetInstance().AddGameVelocity(velocity_.GetLength() * MOVE_ADD_VEL_EXTEND_);
	}

	//ジャンプ中でスペース押しっぱなしだったら
	if (!isOnGround_ && KeyboardInput::GetInstance().KeyPush(DIK_SPACE) && velocity_.GetLength())
	{
		GameVelocityManager::GetInstance().AddGameVelocity(-0.001f);
	}

	//位置セット(ゲームスピードをかける)
	SetTrans(GetTrans() + velocity_ * VELOCITY_TMP_ * GameVelocityManager::GetInstance().GetVelocity());

	//地面との判定
	std::function<void()>gameSpeedAddFunc = [=]() {GameVelocityManager::GetInstance().AddGameVelocity(1.0f); };
	OnGroundAndWallUpdate(4.0f, GameVelocityManager::GetInstance().GetVelocity(), KeyboardInput::GetInstance().KeyPush(DIK_SPACE), gameSpeedAddFunc);

	//コライダー更新
	Object::WorldMatColliderUpdate();

	//カメラをプレイヤーと同じ位置に
	camera->SetEye(GetTrans());

	//カメラの注視点に回転したベクトルセット
	camera->SetTarget(GetTrans() + frontVec_);
}

void Player::Update()
{
	//ゲームオーバーまでの
	if (isDead_)
	{
		SetScale({ 0,0,0 });

		if (deadTimer_ <= 0)
		{
			FallWeapon({ 0,0,0 });
			SetIsAlive(false);
		}

		deadTimer_--;

		//パーティクル
		for (int32_t i = 0; i < 1; ++i)
		{
			const float MD_VEL = 0.6f;
			Vec3 vel{};
			vel.x_ = (float)rand() / RAND_MAX * MD_VEL - MD_VEL / 2.0f;
			vel.y_ = (float)rand() / RAND_MAX * MD_VEL - MD_VEL / 2.0f;
			vel.z_ = (float)rand() / RAND_MAX * MD_VEL - MD_VEL / 2.0f;

			float scale = (float)rand() / RAND_MAX * GetScale().x_;

			ParticleManager::GetInstance()->Add(30, GetTrans(), vel, { 0,0,0 }, scale, 0, { 2.0f,2.0f,2.0f,1.5f }, { 0,0,0,0.0f });
		}

		return;
	}

	//カメラの向き変更
	DirectionUpdate();

	//移動
	Move();

	//素手や銃などのステート
	state_->Update();

	Object::Update();

	//手のアップデート
	handManager_->Update();
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

void Player::DrawImGui()
{
	ImGui::Begin("Player");
	ImGui::Checkbox("ValidDamage", &isValidDamage_);
	ImGui::End();
}


void Player::Dead(const CollisionInfo& info)
{
	//hpが0になったら
	isDead_ = true;

	Bullet* bullet = dynamic_cast<Bullet*>(info.object_);

	////弾の方向へのベクトル
	Vec3 directionVec;
	//所有者がまだ生きていたら
	//if (!bullet->GetOwner()->GetIsAlive())
	{
		directionVec = bullet->GetOwnerPos() - GetTrans();
	}

	//初期正面ベクトルとプレイヤーへのベクトル
	Vec3 fVTmp = GetFrontVecTmp().GetNormalized();
	Vec3 pDVTmp = directionVec.GetNormalized();

	//正面ベクトルから被弾方向ベクトルへの回転クォータニオン
	Quaternion q = Quaternion::DirectionToDirection(fVTmp, pDVTmp);
	//回転後のベクトル
	fVTmp = q.GetRotateVector(fVTmp);
	//正面ベクトルセット
	SetFrontVec(fVTmp);

	//角度じゃなくて行列をそのまま使う
	SetIsUseQuaternionMatRot(true);
	SetMatRot(q.MakeRotateMatrix());

	//行列更新
	Object::WorldMatColliderUpdate();

	//カメラの注視点に回転したベクトルセット
	CameraManager::GetInstance().GetCamera("playerCamera")->SetTarget(GetTrans() + fVTmp.GetNormalized());
	/*CameraManager::GetInstance().GetCamera("playerCamera")->Update();*/
}

void Player::ThrowWeapon()
{
	FallWeapon(GetFrontVec() * FALL_VEL_POW_);

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
