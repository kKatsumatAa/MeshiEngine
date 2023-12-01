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
#include "ObjectManager.h"
#include "LevelManager.h"

using namespace DirectX;


std::unique_ptr<Bullet> Bullet::Create(const Vec3& pos, const Vec3& directionVec, float scale, float lifeTime, IObject* owner)
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

bool Bullet::Initialize(const Vec3& pos, const Vec3& directionVec, float scale, float lifeTime, IObject* owner)
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
	oldPos_ = pos - directionVec.GetNormalized() * 0.01f;

	directionVec_ = directionVec;
	SetScale({ scale,scale,scale });
	lifeTime_ = lifeTime;
	owner_ = owner;
	ownerPos_ = owner->GetTrans();
	//属性
	GetCollider()->SetAttribute(COLLISION_ATTR_ITEMS);

	//弾道モデル
	ballisticsObj_.Initialize();
	ballisticsObj_.SetModel(ModelManager::GetInstance().LoadModel("ballistics"));

	//色セット
	SetColor({ 0.1f,0.1f,0.1f,1.0f });

	Update();

	return true;
}

void Bullet::Dead(const Vec3& interPos, uint64_t attr)
{
	SetIsAlive(false);

	if (attr & COLLISION_ATTR_LANDSHAPE)
	{
		//ステージに波紋
		BeginWaveStage(GetTrans(), Vec2(GetScale().z, GetScale().y) * 35.0f, GetScale().GetLength() * 50.0f, 40.0f);
	}

	//パーティクル
	for (int32_t i = 0; i < 20; ++i)
	{
		Vec3 vel{};
		vel.x = GetRand(-0.2f, 0.2f);
		vel.y = GetRand(-0.2f, 0.2f);
		vel.z = GetRand(-0.2f, 0.2f);

		float scale = GetRand(GetScale().x / 2.0f, GetScale().x * 4.0f);

		ParticleManager::GetInstance()->Add(30, interPos, vel, { 0,0,0 }, scale, 0, { 0,0,0,1.5f }, { 0,0,0,0.0f },
			ParticleManager::BLEND_NUM::TRIANGLE);
	}
}

void Bullet::Dead(const CollisionInfo& info)
{
	Dead({ info.inter_.m128_f32[0],info.inter_.m128_f32[1] ,info.inter_.m128_f32[2] }, info.collider_->GetAttribute());
}

void Bullet::BallisticsUpdate()
{
	ballisticsObj_.SetIsSilhouette(true);

	//位置セット(今の位置から方向のベクトルの逆ベクトルに長さ分進めた場所)
	ballisticsObj_.SetTrans({ GetWorldTrans() - GetFrontVec() * ballisticsLength });

	//ｚのみ弾道の長さを設定
	ballisticsObj_.SetScale({ GetScale().x, GetScale().y,ballisticsLength });

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
	//ゲームスピードをかける
	lifeTime_ -= 1.0f;

	//移動もゲームスピードをかける
	velocity_ = directionVec_ * GameVelocityManager::GetInstance().GetVelocity();
	SetTrans(GetWorldTrans() + velocity_);

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
			//撃った本人には当たらないように
			if (owner_ != nullptr && owner_ != info.object)
			{
				Dead({ info.inter.m128_f32[0],info.inter.m128_f32[1], info.inter.m128_f32[2] }, info.collider->GetAttribute());
			}
		}
	}

	//前回の位置を記録
	oldPos_ = GetTrans() - directionVec_.GetNormalized() * 0.01f;

	//メッシュ分割のウェーブ
	if ((int32_t)lifeTime_ % 20 == 0)
	{
		BeginWaveStage(GetTrans(), Vec2(GetScale().z, GetScale().y) * 5.0f, GetScale().GetLength() * 80.0f, 20.0f);
	}
}

void Bullet::DrawShadow()
{
	DrawSphere(nullptr, nullptr, NULL, Object::PipelineStateNumPrim::SHADOW_PRIM);
}

void Bullet::Draw()
{
	//弾道
	ballisticsObj_.DrawModel(nullptr, nullptr);

	DrawSphere();

	//疑似シルエット解除
	SetIsSilhouette(false);
}


void Bullet::OnCollision(const CollisionInfo& info)
{
	//撃った本人には当たらないように
	if (owner_ != info.object_ && info.object_->GetObjName().find("enemy") == std::string::npos)
	{
		Dead(info);
	}
}
