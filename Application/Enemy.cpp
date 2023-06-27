#include "Enemy.h"
#include "KeyboardInput.h"
#include "DebugText.h"
#include "CameraManager.h"
#include "ParticleManager.h"
#include "PadInput.h"
#include "MouseInput.h"
#include "GameVelocityManager.h"

using namespace DirectX;


std::unique_ptr<Enemy> Enemy::Create()
{
	std::unique_ptr<Enemy> instance = std::make_unique<Enemy>();
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

bool Enemy::Initialize()
{
	if (!Object::Initialize())
	{
		return false;
	}

	//名前
	SetObjName("enemy");

	//コライダーの追加
	//float radius = 0.6f;
	//半径分だけ足元から浮いた座標を球の中心にする

	SetCollider(std::make_unique<SphereCollider>());

	return true;
}

void Enemy::Move()
{
	//動けなかったら飛ばす
	if (isCantMove)
	{
		isCantMove = false;
		return;
	}

	//プレイヤーに向かわせる(とりあえずカメラから位置もらう)
	Vec3 playerPos = CameraManager::GetInstance().GetCamera("debugCamera")->GetEye();
	//敵からプレイヤーへのベクトル
	Vec3 directionVec = playerPos - GetTrans();
	//正規化
	directionVec.Normalized();

	//ゲームスピードをかける
	velocity_ = directionVec * VELOCITY_TMP_ * GameVelocityManager::GetInstance().GetVelocity();

	SetTrans(GetTrans() + velocity_);

	//プレイヤーの方を向かせる
	/*directionRotTime += GameVelocityManager::GetInstance().GetVelocity();
	if (directionRotTime > DIRCTION_ROT_TIME_) { directionRotTime = 0; }
	float t = directionRotTime / DIRCTION_ROT_TIME_;*/

	//CulcFrontVec();

	Quaternion q = Quaternion::DirectionToDirection(
		GetFrontVec().GetNormalized(), directionVec.GetNormalized());

	//角度をセット
	//Vec3 rot = GetRotFromQuaternion(Slerp(GetQuaternion(), q, 1.0f));
	Vec3 rot = GetRotFromQuaternion(q);
	//Vec3 rot = GetRotFromMat(q.MakeRotateMatrix());
	Vec3 rot1 = Object::GetRot();
	SetRot(/*rot1 +*/ rot);
}

void Enemy::Update()
{
	//Move();

	Object::Update();
}


void Enemy::OnCollision(const CollisionInfo& info)
{
	//仮でプレイヤーに当たったら生きてるフラグオフ
	if (info.object_->GetObjName() == "player")
	{
		SetIsAlive(false);

		for (int32_t i = 0; i < 30; ++i)
		{
			const float MD_VEL = 0.1f;
			Vec3 vel{};
			vel.x_ = (float)rand() / RAND_MAX * MD_VEL - MD_VEL / 2.0f;
			vel.y_ = (float)rand() / RAND_MAX * MD_VEL - MD_VEL / 2.0f;
			vel.z_ = (float)rand() / RAND_MAX * MD_VEL - MD_VEL / 2.0f;

			Vec3 pos = { info.inter_.m128_f32[0],info.inter_.m128_f32[1],info.inter_.m128_f32[2] };

			ParticleManager::GetInstance()->Add(180, pos, vel, { 0,0,0 }, 1.0f, 0.0f);
		}
	}
	//敵同士で当たったらめり込まないようにする
	else if (info.object_->GetObjName() == "enemy")
	{
		//長さ
		float length = (info.object_->GetScale().x_ + GetScale().x_);
		//距離のベクトル
		Vec3 distanceVec = GetTrans() - info.object_->GetTrans();
		distanceVec.Normalized();

		//位置セット(半径＋半径の長さをベクトルの方向を使って足す)
		SetTrans(info.object_->GetTrans() + distanceVec * length * 1.001f);
		//動けないようにする
		isCantMove = true;
	}
}
