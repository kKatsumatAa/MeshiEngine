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
	Vec3 playerPos = CameraManager::GetInstance().GetCamera("playerCamera")->GetEye();
	//敵からプレイヤーへのベクトル
	Vec3 directionVec = playerPos - GetTrans();
	//正規化
	//directionVec.y_ = 0;
	directionVec.Normalized();

	//現在のスピードに方向ベクトル足し、ゲームスピードをかける
	velocity_ = GetVelocity() + directionVec * VELOCITY_TMP_ * GameVelocityManager::GetInstance().GetVelocity();
	//スピードの上限は超えないように
	float length = velocity_.GetLength();
	//スピードがプラスになってたら
	if (velocity_.Dot(directionVec) > 0)
	{
		//ダメージのクールリセット
		damageCoolTime = 0;
		//スピードの上限超えない
		velocity_ = velocity_.GetNormalized() * min(length, GameVelocityManager::GetInstance().GetVelocity() * VELOCITY_TMP_);
	}
	//当たり判定用にセット
	SetVelocity(velocity_);

	//位置セット
	SetTrans(GetTrans() + velocity_);

	////プレイヤーの方を向かせる
	//directionRotTime += GameVelocityManager::GetInstance().GetVelocity();
	//if (directionRotTime > DIRCTION_ROT_TIME_) { directionRotTime = 0; }
	//float t = directionRotTime / DIRCTION_ROT_TIME_;

	//敵からプレイヤーへのベクトル
	directionVec = playerPos - GetTrans();
	//初期正面ベクトルとプレイヤーへのベクトル
	Vec3 fVTmp = GetFrontVecTmp().GetNormalized();
	Vec3 pDVTmp = directionVec.GetNormalized();

	//正面ベクトルからプレイヤーの方向ベクトルへの回転クォータニオン
	Quaternion q = Quaternion::DirectionToDirection(fVTmp, pDVTmp);
	//回転後のベクトル
	fVTmp = q.GetRotateVector(fVTmp);
	//正面ベクトルセット
	SetFrontVec(fVTmp);

	//角度じゃなくて行列をそのまま使う
	SetIsUseQuaternionMatRot(true);
	SetMatRot(q.MakeRotateMatrix());
}

void Enemy::Update()
{
	//移動
	Move();

	//ダメージ受けるクールタイムもゲームスピードをかける
	damageCoolTime -= 1.0f * GameVelocityManager::GetInstance().GetVelocity();

	Object::Update();
}


void Enemy::OnCollision(const CollisionInfo& info)
{
	//仮でプレイヤーに当たったら生きてるフラグオフ
	if (info.object_->GetObjName() == "player")
	{
		////長さ
		float length = (/*info.object_->GetScale().x_*/ +GetScale().x_);
		//距離のベクトル
		Vec3 distanceVec = GetTrans() - info.object_->GetTrans();
		//仮でyは動かさない
		distanceVec.y_ = 0;
		distanceVec.Normalized();

		//めり込まないように位置セット(半径＋半径の長さをベクトルの方向を使って足す)
		/*Vec3 ansPos = info.object_->GetTrans() + distanceVec * length * 1.001f;
		SetTrans(ansPos);*/

		//動けないようにする
		isCantMove = true;
	}
	//プレイヤーの攻撃との判定
	else if (info.object_->GetObjName() == "playerAttack")
	{
		if (damageCoolTime <= 0)
		{
			//hp減らす
			hp--;

			//長さ
			float length = (info.object_->GetScale().x_ + GetScale().x_);
			//距離のベクトル
			Vec3 distanceVec = GetTrans() - info.object_->GetTrans();
			//仮
			distanceVec.y_ = 0;
			velocity_.y_ = 0;
			distanceVec.Normalized();
			//ノックバック
			velocity_ += distanceVec * length * 0.5f;
			SetVelocity(velocity_);
			//ダメージを受けるクールタイム
			damageCoolTime = 20;

			//死亡
			if (hp <= 0)
			{
				SetIsAlive(false);
			}

			//パーティクル
			for (int32_t i = 0; i < 30; ++i)
			{
				const float MD_VEL = 0.1f;
				Vec3 vel{};
				vel.x_ = (float)rand() / RAND_MAX * MD_VEL - MD_VEL / 2.0f;
				vel.y_ = (float)rand() / RAND_MAX * MD_VEL - MD_VEL / 2.0f;
				vel.z_ = (float)rand() / RAND_MAX * MD_VEL - MD_VEL / 2.0f;

				Vec3 pos = { info.inter_.m128_f32[0],info.inter_.m128_f32[1],info.inter_.m128_f32[2] };

				float scale = (float)rand() / RAND_MAX;
				float scale2 = (float)rand() / RAND_MAX;

				ParticleManager::GetInstance()->Add(100, pos, vel, { 0,0,0 }, scale, scale2, { 2.0f,0,0,1.5f }, { 0,0,0,0.0f });
			}
		}
	}
	//敵同士で当たったらめり込まないようにする
	else if (info.object_->GetObjName() == "enemy")
	{
		//自分の方が早かったら自分はよけない
		/*if (GetVelocity().GetLength() > info.object_->GetVelocity().GetLength())
		{
			return;
		}*/

		//長さ
		float length = (info.object_->GetScale().x_ + GetScale().x_);
		//距離のベクトル
		Vec3 distanceVec = GetTrans() - info.object_->GetTrans();
		//仮
		distanceVec.y_ = 0;
		distanceVec.Normalized();
		//位置セット(半径＋半径の長さをベクトルの方向を使って足す)
		SetTrans(info.object_->GetTrans() + distanceVec * length * 1.001f);
		////動けないようにする
		//isCantMove = true;

		//ｙは動かないようにする
		SetVelocity({ GetVelocity().x_,0,GetVelocity().z_ });
		info.object_->SetVelocity({ info.object_->GetVelocity().x_,0,info.object_->GetVelocity().z_ });
		//二つのベクトルの合計の長さ
		float addLength = GetVelocity().GetLength() + info.object_->GetVelocity().GetLength();
		//自分のスピードのベクトルの長さの割合（合計の長さで割る（0～1.0f））
		float myLengthRatio = GetVelocity().GetLength() / addLength;

		//衝突後の自分のスピードベクトルは[現在のスピードベクトル]+[相手から自分へのベクトル]*[相手の長さの割合]
		SetVelocity((GetVelocity() + distanceVec.GetNormalized() * addLength * (1.0f - myLengthRatio)) * 0.5f);
		//衝突後の相手のスピードベクトルは[現在のスピードベクトル]+[このインスタンスから相手へのベクトル]*[このインスタンスの長さの割合]
		info.object_->SetVelocity((info.object_->GetVelocity() - distanceVec.GetNormalized() * addLength * (myLengthRatio)) * 0.5f);

		////ベクトルを足して
		//Vec3 addVel = GetVelocity() + info.object_->GetVelocity() * 2.0f;
		//SetVelocity(addVel);
		//info.object_->SetVelocity(addVel);
	}
}
