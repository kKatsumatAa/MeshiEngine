#include "Bullet.h"
#include "KeyboardInput.h"
#include "DebugText.h"
#include "CameraManager.h"
#include "ParticleManager.h"
#include "PadInput.h"
#include "MouseInput.h"
#include "GameVelocityManager.h"
#include "CollisionManager.h"

using namespace DirectX;


std::unique_ptr<Bullet> Bullet::Create(const Vec3& pos, const Vec3& directionVec, float scale, float lifeTime, Object* owner)
{
	std::unique_ptr<Bullet> instance = std::make_unique<Bullet>();
	if (instance.get() == nullptr)
	{
		return nullptr;
	}

	//初期化
	if (!instance->Initialize(pos, directionVec, scale, lifeTime, owner))
	{
		assert(0);
	}

	return std::move(instance);
}

bool Bullet::Initialize(const Vec3& pos, const Vec3& directionVec, float scale, float lifeTime, Object* owner)
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
	//仮で銃はキャラの手前にあるので少し後ろをレイの始点にするため
	oldPos_ = GetTrans() - directionVec;

	directionVec_ = directionVec;
	SetScale({ scale,scale,scale });
	lifeTime_ = lifeTime;
	owner_ = owner;
	ownerPos_ = owner->GetTrans();
	//撃った人の判定属性を自分の属性に
	GetCollider()->SetAttribute(owner->GetCollider()->GetAttribute());

	return true;
}

void Bullet::Dead(const Vec3& interPos)
{
	SetIsAlive(false);

	//パーティクル
	for (int32_t i = 0; i < 20; ++i)
	{
		Vec3 vel{};
		vel.x_ = GetRand(-0.2f, 0.2f);
		vel.y_ = GetRand(-0.2f, 0.2f);
		vel.z_ = GetRand(-0.2f, 0.2f);

		float scale = GetRand(GetScale().x_ / 2.0f, GetScale().x_ * 4.0f);

		ParticleManager::GetInstance()->Add(30, interPos, vel, { 0,0,0 }, scale, 0, { 0,0,0,1.5f }, { 0,0,0,0.0f });
	}
}

void Bullet::Update()
{
	//クールタイムもゲームスピードをかける
	lifeTime_ -= 1.0f * GameVelocityManager::GetInstance().GetVelocity();

	//移動もゲームスピードをかける
	SetTrans(GetTrans() + directionVec_ * GameVelocityManager::GetInstance().GetVelocity());

	Object::Update();

	//前回の位置から今の位置のベクトルをレイとして判定
	{
		RaycastHit info;

		if (CollisionManager::GetInstance()->RaycastUtil(GetTrans(), oldPos_, COLLISION_ATTR_LANDSHAPE, &info))
		{
			Dead({ info.inter.m128_f32[0],info.inter.m128_f32[1], info.inter.m128_f32[2] });
		}
	}

	//前回の位置を記録
	oldPos_ = GetTrans();


	//弾道
	ParticleManager::GetInstance()->Add(15, GetWorldTrans(), { 0,0,0 }, { 0,0,0 }, GetScale().x_ * 3.0f, 0, { 5.0f,0,0,4.5f }, { 5.0f,0,0,4.5f });

	//生存時間超えたら
	if (lifeTime_ <= 0)
	{
		SetIsAlive(false);
	}
}

void Bullet::Draw()
{
	DrawSphere(nullptr, { 0.0f,0.0f,0.0f,1.0f });

	//疑似シルエット解除
	SetIsSilhouette(false);
}


void Bullet::OnCollision(const CollisionInfo& info)
{
	Dead({ info.inter_.m128_f32[0],info.inter_.m128_f32[1], info.inter_.m128_f32[2] });
}
