#include "Bullet.h"
#include "KeyboardInput.h"
#include "DebugText.h"
#include "CameraManager.h"
#include "ParticleManager.h"
#include "PadInput.h"
#include "MouseInput.h"
#include "GameVelocityManager.h"
#include "CollisionManager.h"
#include "ModelManager.h"

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

	//弾道モデル
	ballisticsObj_.Initialize();
	ballisticsObj_.SetModel(ModelManager::GetInstance().LoadModel("ballistics"));

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

void Bullet::BallisticsUpdate()
{
	ballisticsObj_.SetIsSilhouette(true);

	//位置セット(今の位置から方向のベクトルの逆ベクトルに長さ分進めた場所)
	ballisticsObj_.SetTrans({ GetWorldTrans() - GetFrontVec() * ballisticsLength });

	//ｚのみ弾道の長さを設定
	ballisticsObj_.SetScale({ GetScale().x_, GetScale().y_,ballisticsLength });

	//正面の基ベクトルはｚ軸の奥
	ballisticsObj_.SetFrontVecTmp({ 0,0,1.0f });

	//弾が向かってる方向に回転させるクォータニオン
	Quaternion directionQ = Quaternion::DirectionToDirection(ballisticsObj_.GetFrontVecTmp(), GetFrontVec(), -1.0f);

	//行列をそのまま使う
	ballisticsObj_.SetIsUseQuaternionMatRot(true);
	ballisticsObj_.SetMatRot(directionQ.MakeRotateMatrix());

	ballisticsObj_.Update();

	//弾道を少しずつ伸ばす
	ballisticsLength += GameVelocityManager::GetInstance().GetVelocity();
	ballisticsLength = min(ballisticsLength, BALLISTICS_LENGTH_MAX_);
}

void Bullet::Update()
{
	//クールタイムもゲームスピードをかける
	lifeTime_ -= 1.0f * GameVelocityManager::GetInstance().GetVelocity();

	//移動もゲームスピードをかける
	SetTrans(GetTrans() + directionVec_ * GameVelocityManager::GetInstance().GetVelocity());

	Object::Update();

	//正面ベクトルには向かってる方向のベクトル
	SetFrontVec((GetWorldTrans() - oldPos_).GetNormalized());

	//弾道
	BallisticsUpdate();

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

	//生存時間超えたら
	if (lifeTime_ <= 0)
	{
		SetIsAlive(false);
	}
}

void Bullet::Draw()
{
	//弾道
	ballisticsObj_.DrawModel(nullptr, nullptr, { 3.0f,0,0,0.9f });

	DrawSphere(nullptr, { 0.0f,0.0f,0.0f,1.0f });

	//疑似シルエット解除
	SetIsSilhouette(false);
}


void Bullet::OnCollision(const CollisionInfo& info)
{
	Dead({ info.inter_.m128_f32[0],info.inter_.m128_f32[1], info.inter_.m128_f32[2] });
}
