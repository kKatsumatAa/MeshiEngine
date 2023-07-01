#include "Bullet.h"
#include "KeyboardInput.h"
#include "DebugText.h"
#include "CameraManager.h"
#include "ParticleManager.h"
#include "PadInput.h"
#include "MouseInput.h"
#include "GameVelocityManager.h"

using namespace DirectX;


std::unique_ptr<Bullet> Bullet::Create(const Vec3& pos, const Vec3& directionVec, float scale, float lifeTime)
{
	std::unique_ptr<Bullet> instance = std::make_unique<Bullet>();
	if (instance.get() == nullptr)
	{
		return nullptr;
	}

	//初期化
	if (!instance->Initialize(pos, directionVec, scale, lifeTime))
	{
		assert(0);
	}

	return std::move(instance);
}

bool Bullet::Initialize(const Vec3& pos, const Vec3& directionVec, float scale, float lifeTime)
{
	if (!Object::Initialize())
	{
		return false;
	}

	//名前
	SetObjName("bullet");

	//コライダーの追加
	SetCollider(std::make_unique<SphereCollider>());

	SetTrans(pos);
	directionVec_ = directionVec;
	SetScale({ scale,scale,scale });
	lifeTime_ = lifeTime;

	return true;
}

void Bullet::Update()
{
	//クールタイムもゲームスピードをかける
	lifeTime_ -= 1.0f * GameVelocityManager::GetInstance().GetVelocity();

	//移動もゲームスピードをかける
	SetTrans(GetTrans() + directionVec_ * GameVelocityManager::GetInstance().GetVelocity());

	Object::Update();


	if (lifeTime_ <= 0)
	{
		isAlive_ = false;
	}
}

void Bullet::Draw()
{
	DrawSphere();
}


void Bullet::OnCollision(const CollisionInfo& info)
{
	isAlive_ = false;

	//パーティクル
	for (int32_t i = 0; i < 20; ++i)
	{
		const float MD_VEL = 0.2f;
		Vec3 vel{};
		vel.x_ = (float)rand() / RAND_MAX * MD_VEL - MD_VEL / 2.0f;
		vel.y_ = (float)rand() / RAND_MAX * MD_VEL - MD_VEL / 2.0f;
		vel.z_ = (float)rand() / RAND_MAX * MD_VEL - MD_VEL / 2.0f;

		Vec3 pos = { info.inter_.m128_f32[0],info.inter_.m128_f32[1],info.inter_.m128_f32[2] };

		float scale = (float)rand() / RAND_MAX * GetScale().x_;

		ParticleManager::GetInstance()->Add(30, pos, vel, { 0,0,0 }, scale, 0, { 2.0f,2.0f,2.0f,1.5f }, { 0,0,0,0.0f });
	}
}
