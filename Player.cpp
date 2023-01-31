#include "Player.h"
#include "KeyboardInput.h"
#include "DebugText.h"
#include "SphereCollider.h"
#include "ParticleManager.h"
#include "PadInput.h"

using namespace DirectX;


Player* Player::Create()
{
	Player* instance = new Player();
	if (instance == nullptr)
	{
		return nullptr;
	}

	//初期化
	if (!instance->Initialize())
	{
		delete instance;
		assert(0);
	}

	return instance;
}

bool Player::Initialize()
{
	if (!Object::Initialize())
	{
		return false;
	}

	//コライダーの追加
	float radius = 0.6f;
	//半径分だけ足元から浮いた座標を球の中心にする
	SetCollider(new SphereCollider(XMVECTOR({ 0,radius,0,0 }), radius));

	return true;
}

void Player::Update()
{
	KeyboardInput* input = &KeyboardInput::GetInstance();

	if (input->KeyPush(DIK_LEFTARROW) || PadInput::GetInstance().GetRightStickTilt().x < 0)
	{
		worldMat->rot.y -= AngletoRadi(2.0f);
	}
	if (input->KeyPush(DIK_RIGHTARROW) || PadInput::GetInstance().GetRightStickTilt().x > 0)
	{
		worldMat->rot.y += AngletoRadi(2.0f);
	}

	//移動ベクトルをY軸周りの角度で回転
	Vec3 move = { 0,0,1.0f };
	worldMat->SetWorld();
	Vec3xM4(move, worldMat->matWorld, 0);

	//向いてる方向に移動
	if (input->KeyPush(DIK_UPARROW) || PadInput::GetInstance().GetLeftStickTilt().y < 0)
	{
		worldMat->trans.x -= move.x;
		worldMat->trans.y -= move.y;
		worldMat->trans.z -= move.z;
	}
	if (input->KeyPush(DIK_DOWNARROW) || PadInput::GetInstance().GetLeftStickTilt().y > 0)
	{
		worldMat->trans.x += move.x;
		worldMat->trans.y += move.y;
		worldMat->trans.z += move.z;
	}

	Object::Update();
}


void Player::OnCollision(const CollisionInfo& info)
{
	for (int i = 0; i < 1; ++i)
	{
		const float md_vel = 0.5f;
		XMFLOAT3 vel{};
		vel.x = (float)rand() / RAND_MAX * md_vel - md_vel / 2.0f;
		vel.y = (float)rand() / RAND_MAX * md_vel - md_vel / 2.0f;
		vel.z = (float)rand() / RAND_MAX * md_vel - md_vel / 2.0f;

		ParticleManager::GetInstance()->Add(20, XMFLOAT3(info.inter.m128_f32), vel, XMFLOAT3(), 0, 1.0f);
	}
}
