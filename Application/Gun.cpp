#include "Gun.h"
#include "KeyboardInput.h"
#include "DebugText.h"
#include "CameraManager.h"
#include "ParticleManager.h"
#include "PadInput.h"
#include "MouseInput.h"
#include "GameVelocityManager.h"
#include "BulletManager.h"

using namespace DirectX;


std::unique_ptr<Gun> Gun::Create(std::unique_ptr<WorldMat> worldMat)
{
	std::unique_ptr<Gun> instance = std::make_unique<Gun>();
	if (instance.get() == nullptr)
	{
		return nullptr;
	}

	//初期化
	if (!instance->Initialize(std::move(worldMat)))
	{
		assert(0);
	}

	return std::move(instance);
}

bool Gun::Initialize(std::unique_ptr<WorldMat> worldMat)
{
	if (!Object::Initialize(std::move(worldMat)))
	{
		return false;
	}

	//名前
	SetObjName("gun");

	//コライダーの追加
	//float radius = 0.6f;
	//半径分だけ足元から浮いた座標を球の中心にする

	SetCollider(std::make_unique<SphereCollider>());

	//ローカル座標
	localPos_ = GetTrans();

	return true;
}

void Gun::Attack(const Vec3& directionVec, int32_t decreBullet)
{
	//クールタイムじゃなく、残弾なかったら抜ける
	if (shotCoolTime_ > 0 || remainingBullets_ <= 0)
	{
		return;
	}

	//弾うつ処理
	BulletManager::GetInstance().CreateBullet(shotPos_, directionVec.GetNormalized() * BULLET_VELOCITY_, GetScale().x_ * 0.7f, 300);

	//パーティクル
	ParticleGenerate();

	shotCoolTime_ = SHOT_COOL_TIME_MAX_;
	remainingBullets_ -= decreBullet;
}

void Gun::ChangeOwner(WorldMat* parent)
{
	SetRot({ 0,0,0 });
	remainingBullets_ = BULLETS_TMP_;
	shotCoolTime_ = 0;

	if (parent == nullptr)
	{
		//離れた瞬間にワールド座標を入れる(親がいなくなるので)
		SetTrans(GetWorldTrans());
	}
	else
	{
		//拾われ、親がはいったらローカル座標に切り替え
		SetTrans(localPos_);
	}


	SetParent(parent);
}

void Gun::Update()
{
	//発射座標
	shotPos_ = { GetWorldTrans().x_, GetWorldTrans().y_ + GetScale().y_, GetWorldTrans().z_ - GetScale().z_ };

	//親が亡くなった後の動き
	NoParentMove();
	//地面より下に行かないように
	if (GetParent() == nullptr)
	{
		SetTransY(max(GetTrans().y_, GetScale().x_));
	}

	//クールタイムもゲームスピードをかける
	shotCoolTime_ -= 1.0f * GameVelocityManager::GetInstance().GetVelocity();

	Object::Update();
}


//----------------------------------------------------------------------------------------------------------------
void Gun::ParticleGenerate()
{
	//パーティクル
	for (int32_t i = 0; i < 40; ++i)
	{
		const float MD_VEL = 0.6f;
		Vec3 vel{};
		vel.x_ = (float)rand() / RAND_MAX * MD_VEL - MD_VEL / 2.0f;
		vel.y_ = (float)rand() / RAND_MAX * MD_VEL - MD_VEL / 2.0f;
		vel.z_ = (float)rand() / RAND_MAX * MD_VEL - MD_VEL / 2.0f;

		float scale = GetRand(GetScale().x_ / 2.0f, GetScale().x_ * 2.0f);

		ParticleManager::GetInstance()->Add(40, shotPos_, vel, { 0,0,0 }, scale, 0, { 4.0f,4.0f,4.0f,1.5f }, { 0,0,0,0.0f });
	}

}

void Gun::OnCollision(const CollisionInfo& info)
{
	if (info.object_->GetObjName() == "enemy")
	{
		//持ってる人がいるときはしない
		if (GetParent() && fallVec_.GetLength() == 0)
		{
			return;
		}

		//パーティクル
		ParticleGenerate();

		SetIsAlive(false);
	}
}
