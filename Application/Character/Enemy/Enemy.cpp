#include "Enemy.h"
#include "KeyboardInput.h"
#include "DebugText.h"
#include "CameraManager.h"
#include "ParticleManager.h"
#include "PadInput.h"
#include "MouseInput.h"
#include "GameVelocityManager.h"

using namespace DirectX;


std::unique_ptr<Enemy> Enemy::Create(std::unique_ptr<WorldMat> worldMat, int32_t waveNum, Weapon* weapon)
{
	std::unique_ptr<Enemy> instance = std::make_unique<Enemy>();
	if (instance.get() == nullptr)
	{
		return nullptr;
	}

	//初期化
	if (!instance->Initialize(std::move(worldMat), waveNum, weapon))
	{
		assert(0);
	}

	return std::move(instance);
}

bool Enemy::Initialize(std::unique_ptr<WorldMat> worldMat, int32_t waveNum, Weapon* weapon)
{
	if (!Object::Initialize(std::move(worldMat)))
	{
		return false;
	}


	if (weapon)
	{
		weapon_ = weapon;
		//武器を諸々設定
		PickUpWeapon(weapon_);
	}

	hp_ = HP_TMP_;

	waveNum_ = waveNum;

	//ディゾルブ
	SetisDissolve(true);
	//ディゾルブ画像
	uint64_t handle;
	handle = TextureManager::LoadGraph("dissolveMask.png");
	SetDissolveTexHandle(handle);


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
	directionVec = playerPos - GetTrans();

	//正規化
	//directionVec.y_ = 0;
	directionVec.Normalized();

	//現在のスピードに方向ベクトル足し、ゲームスピードをかける
	velocity_ = GetVelocity() + directionVec * VELOCITY_TMP_ * GameVelocityManager::GetInstance().GetVelocity();
	//スピードの上限は超えないように
	float length = velocity_.GetLength();
	//スピードがプラスになってたら
	if (velocity_.Dot(directionVec) >= 0)
	{
		directionVec = playerPos - GetTrans();
		//ある程度近づいたら止まる
		if (directionVec.GetLength() < GetScale().GetLength() * 1.5f)
		{
			velocity_ = { 0,0,0 };
		}
		directionVec.Normalized();

		//ダメージのクールリセット
		damageCoolTime = 0;
		//スピードの上限超えない
		velocity_ = velocity_.GetNormalized() * min(length, GameVelocityManager::GetInstance().GetVelocity() * VELOCITY_TMP_ / 1.5f);
	}

	//当たり判定用にセット
	SetVelocity(velocity_);

	//位置セット
	SetTrans(GetTrans() + velocity_);

	//地面と壁との判定
	OnGroundAndWallUpdate(GetScale().y_, GameVelocityManager::GetInstance().GetVelocity());


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
	if (GetModel())
	{
		GetModel()->SetMaterialExtend({ 1.0f,8.0f,20.0f });
	}

	//移動
	Move();

	//武器で攻撃
	if (weapon_ != nullptr && weapon_->GetIsAlive())
	{
		Vec3 playerPos = CameraManager::GetInstance().GetCamera("playerCamera")->GetEye();
		Vec3 directionV = playerPos - weapon_->GetWorldTrans();
		weapon_->Attack(directionV.GetNormalized(), 0, this);
	}

	//hpによってディゾルブ
	SetDissolveT((1.0f - (float)hp_ / (float)HP_TMP_) * DISSOLVE_POW_);

	//ダメージ受けるクールタイムもゲームスピードをかける
	damageCoolTime -= 1.0f * GameVelocityManager::GetInstance().GetVelocity();

	//アニメーションもゲームスピード
	SetAnimationSpeed(GameVelocityManager::GetInstance().GetVelocity() * 3.0f);

	Character::Update();
}

void Enemy::Draw()
{
	Object::DrawModel(nullptr, nullptr, { 3.0f,0,0,1.0f });
}


//----------------------------------------------------------------------------------
void Enemy::KnockBack(const CollisionInfo& info)
{
	//長さ
	float length = (info.object_->GetScale().x_ + GetScale().x_);
	//距離のベクトル
	Vec3 distanceVec = GetTrans() - info.object_->GetTrans();
	//仮
	distanceVec.y_ = 0;
	velocity_.y_ = 0;
	distanceVec.Normalized();
	//ノックバック
	velocity_ += distanceVec * length * KNOCK_BACK_POW_;
	SetVelocity(velocity_);
	//ダメージを受けるクールタイム
	damageCoolTime = 20;

	//武器持っていたら落とす
	if (weapon_)
	{
		distanceVec.y_ = 0.2f;
		FallWeapon({ -distanceVec.x_ * WEAPON_FALL_VEL_EXTEND_,distanceVec.y_,-distanceVec.z_ * WEAPON_FALL_VEL_EXTEND_ });
	}
}

void Enemy::DamageParticle(const CollisionInfo& info)
{
	for (int32_t i = 0; i < 80; ++i)
	{
		Vec3 pos = { info.inter_.m128_f32[0],info.inter_.m128_f32[1],info.inter_.m128_f32[2] };

		float scaleTmp = GetScale().GetLength();

		Vec3 addPos = Vec3(GetRand(-GetScale().x_, GetScale().x_), GetRand(-GetScale().y_, GetScale().y_), GetRand(-GetScale().z_, GetScale().z_)) / 2.0f;

		pos += addPos;

		const int32_t LIFE_TIME = 40;

		Vec3 vel = /*(GetTrans() - pos) * */Vec3(GetRand(-0.5f, 0.5f), GetRand(-0.5f, 0.5f), GetRand(-0.5f, 0.5f)) / 2.0f;

		float scale = GetRand(scaleTmp / 50.0f, scaleTmp / 15.0f);
		float scale2 = GetRand(0, scaleTmp / 80.0f);

		ParticleManager::GetInstance()->Add(LIFE_TIME, pos, vel, { 0,-0.002f,0 }, scale, scale2, { 3.0f,0.0f,0.0f,0.95f }, { 0,0,0,0.0f },
			PI * 10.0f, -PI * 10.0f);
	}
}

void Enemy::OnCollision(const CollisionInfo& info)
{
	//プレイヤーに当たったら
	if (info.object_->GetObjName() == "player")
	{
		////長さ
		float length = (info.object_->GetScale().x_ + GetScale().x_);
		//距離のベクトル
		Vec3 distanceVec = GetTrans() - info.object_->GetTrans();
		//仮でyは動かさない
		distanceVec.y_ = 0;
		distanceVec.Normalized();

		//めり込まないように位置セット(半径＋半径の長さをベクトルの方向を使って足す)
		//Vec3 ansPosP = info.object_->GetTrans() + distanceVec * length * 0.5f;
		Vec3 ansPosE = info.object_->GetTrans() + distanceVec * length * 1.001f;
		//SetTrans(ansPosP);
		SetTrans(ansPosE);

		//info.object_->SetTrans(ansPos);
		SetVelocity({ 0,0,0 });

		//動けないようにする
		isCantMove = true;
	}
	//プレイヤーの攻撃との判定
	else if (info.object_->GetObjName() == "playerAttack")
	{
		if (damageCoolTime <= 0)
		{
			//ノックバック
			KnockBack(info);

			//hp減らす
			Damaged(1, [=]() {SetIsAlive(false); });

			//パーティクル
			DamageParticle(info);
		}
	}
	//弾に当たったら
	else if (info.object_->GetObjName() == "bullet")
	{
		//今のhp分ダメージ受けて倒れる
		Damaged(hp_, [=]() {SetIsAlive(false); });

		//ノックバック
		KnockBack(info);

		//パーティクル
		DamageParticle(info);
	}
	//銃に当たったら
	else if (info.object_->GetObjName() == "gun")
	{
		Gun* gun = dynamic_cast<Gun*>(info.object_);

		//投げられているときのみ
		if (gun != nullptr && gun->GetParent() == nullptr && gun->GetIsThrowing() && gun->GetFallVelocity().GetLength() != 0)
		{
			//
			KnockBack(info);
		}
	}
	//敵同士で当たったらめり込まないようにする
	else if (info.object_->GetObjName().find("enemy") != std::string::npos)
	{
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
		SetVelocity((GetVelocity() + distanceVec.GetNormalized() * addLength * (1.0f - myLengthRatio)) * 0.63f);
		//衝突後の相手のスピードベクトルは[現在のスピードベクトル]+[このインスタンスから相手へのベクトル]*[このインスタンスの長さの割合]
		info.object_->SetVelocity((info.object_->GetVelocity() - distanceVec.GetNormalized() * addLength * (myLengthRatio)) * 0.63f);
	}
}
