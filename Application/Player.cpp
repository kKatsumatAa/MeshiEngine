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

void Player::Move()
{
	//キー入力
	KeyboardInput* input = &KeyboardInput::GetInstance();

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
	rotWorldMat.rot_ = (rotWorldMat.rot_ + rotMove);
	rotWorldMat.rot_.x_ = (min(rotWorldMat.rot_.x_, PI / 2.0f * 0.9f));
	rotWorldMat.rot_.x_ = (max(rotWorldMat.rot_.x_, -PI / 2.0f * 0.9f));

	//移動ベクトルをY軸周りの角度で回転
	Vec3 frontVec = { 0,0,1.0f };
	Vec3 upVec = { 0,1.0f,0 };
	Vec3 rightVec = { 0,0,0 };
	rotWorldMat.CulcWorldMat();
	//回転
	Vec3xM4(frontVec, rotWorldMat.matWorld_, 0);

	//カメラの右方向ベクトルを出す
	rightVec = upVec.Cross(frontVec);
	//上方向も正しいものを
	//upVec = frontVec.Cross(rightVec);

	//カメラの上方向セット
	//camera->SetUp(upVec);


	Vec3 velocity_ = { 0,0,0 };
	//向いてる方向に移動
	if (input->KeyPush(DIK_UPARROW) || input->KeyPush(DIK_W) || PadInput::GetInstance().GetLeftStickTilt().y_ < 0)
	{
		velocity_ += { frontVec.x_, 0, frontVec.z_ };
	}
	if (input->KeyPush(DIK_DOWNARROW) || input->KeyPush(DIK_S) || PadInput::GetInstance().GetLeftStickTilt().y_ > 0)
	{
		velocity_ += { -frontVec.x_, 0, -frontVec.z_ };
	}
	if (input->KeyPush(DIK_LEFTARROW) || input->KeyPush(DIK_A) || PadInput::GetInstance().GetLeftStickTilt().y_ < 0)
	{
		velocity_ += { -rightVec.x_, 0, -rightVec.z_ };
	}
	if (input->KeyPush(DIK_RIGHTARROW) || input->KeyPush(DIK_D) || PadInput::GetInstance().GetLeftStickTilt().y_ > 0)
	{
		velocity_ += { rightVec.x_, 0, rightVec.z_ };
	}

	//ゲームスピードを移動で足す
	GameVelocityManager::GetInstance().AddGameVelocity(velocity_.GetLength() * 10.0f);

	//位置セット(ゲームスピードをかける)
	SetTrans(GetTrans() + velocity_ * GameVelocityManager::GetInstance().GetVelocity());

	//カメラをプレイヤーと同じ位置に
	camera->SetEye(GetTrans());

	//カメラの注視点に回転したベクトルセット
	camera->SetTarget(GetTrans() + frontVec);
}

void Player::Update()
{
	//仮で小さくする
	SetScale({ 0,0,0 });

	Move();

	Object::Update();
}


void Player::OnCollision(const CollisionInfo& info)
{
	for (int32_t i = 0; i < 1; ++i)
	{
		const float MD_VEL = 0.5f;
		Vec3 vel{};
		vel.x_ = (float)rand() / RAND_MAX * MD_VEL - MD_VEL / 2.0f;
		vel.y_ = (float)rand() / RAND_MAX * MD_VEL - MD_VEL / 2.0f;
		vel.z_ = (float)rand() / RAND_MAX * MD_VEL - MD_VEL / 2.0f;

		Vec3 pos = { info.inter_.m128_f32[0],info.inter_.m128_f32[1],info.inter_.m128_f32[2] };

		ParticleManager::GetInstance()->Add(180, pos, vel, { 0,0,0 }, 1.0f, 0.0f);
	}
}
