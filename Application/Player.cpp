#include "Player.h"
#include "KeyboardInput.h"
#include "DebugText.h"
#include "CameraManager.h"
#include "ParticleManager.h"
#include "PadInput.h"
#include "MouseInput.h"
#include "GameVelocityManager.h"

using namespace DirectX;


std::unique_ptr<Player> Player::Create()
{
	std::unique_ptr<Player> instance = std::make_unique<Player>();
	if (instance.get() == nullptr)
	{
		return nullptr;
	}

	//初期化
	if (!instance->Initialize())
	{
		assert(0);
	}

	return std::move(instance);
}

bool Player::Initialize()
{
	if (!Object::Initialize())
	{
		return false;
	}

	SetObjName("player");

	//コライダーの追加
	float radius = 0.6f;
	//半径分だけ足元から浮いた座標を球の中心にする

	SetCollider(std::make_unique<SphereCollider>(XMVECTOR({ 0,radius,0,0 }), radius));

	//カメラの位置と合わせる
	SetTrans(CameraManager::GetInstance().GetCamera("playerCamera")->GetEye());

	return true;
}

void Player::DirectionUpdate()
{
	//カメラ取得（借りてるだけ）
	Camera* camera = CameraManager::GetInstance().GetCamera("playerCamera");

	//マウスの動きでカメラ角度変更
	Vec2 vel = MouseInput::GetInstance().GetCursorVelocity();

	//マウスの動きでゲームスピードを足す
	GameVelocityManager::GetInstance().AddGameVelocity(vel.GetLength(), "mouse");

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

	//ゲームスピードを移動で足す
	GameVelocityManager::GetInstance().AddGameVelocity(velocity_.GetLength() * 10.0f);

	//位置セット(ゲームスピードをかける)
	SetTrans(GetTrans() + velocity_ * GameVelocityManager::GetInstance().GetVelocity());

	//カメラをプレイヤーと同じ位置に
	camera->SetEye(GetTrans());

	//カメラの注視点に回転したベクトルセット
	camera->SetTarget(GetTrans() + frontVec_);
}

void Player::Update()
{
	//仮で小さくする
	SetScale({ 0,0,0 });

	//カメラの向き変更
	DirectionUpdate();

	//移動
	Move();

	Object::Update();
}


void Player::OnCollision(const CollisionInfo& info)
{
	if (info.object_->GetObjName() == "enemy")
	{
		//パーティクル
		for (int32_t i = 0; i < 30; ++i)
		{
			const float MD_VEL = 0.1f;
			Vec3 vel{};
			vel.x_ = (float)rand() / RAND_MAX * MD_VEL - MD_VEL / 2.0f;
			vel.y_ = (float)rand() / RAND_MAX * MD_VEL - MD_VEL / 2.0f;
			vel.z_ = (float)rand() / RAND_MAX * MD_VEL - MD_VEL / 2.0f;

			Vec3 pos = info.object_->GetTrans();

			ParticleManager::GetInstance()->Add(180, pos, vel, { 0,0,0 }, 0.05f, 0.0f, { 2.0f,2.0f,2.0f,0.8f }, { 0,0,0,0 });
		}


		////長さ
		float length = (info.object_->GetScale().x_ + GetScale().x_);
		//距離のベクトル
		Vec3 distanceVec = GetTrans() - info.object_->GetTrans();
		//仮でyは動かさない
		distanceVec.y_ = 0;
		distanceVec.Normalized();

		//めり込まないように位置セット(半径＋半径の長さをベクトルの方向を使って足す)
		Vec3 ansPos = info.object_->GetTrans() + distanceVec * length * 0.5f;
		SetTrans(ansPos);
		ansPos = info.object_->GetTrans() - distanceVec * length * 0.501f;
		info.object_->SetTrans(ansPos);
	}
}
