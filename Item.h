#pragma once
#include"Collider.h"
#include"Draw.h"
#include"Player.h"
#include"Particle.h"

class Item : public Collider
{
private:
	//モデル
	Draw draw;
	Draw draw2;
	Draw lockOn;
	Draw lockOnLine;

	float lockOnLength = 0;
	Vec3 lockOnVec;


	const float lockOnScaleTmp = 1.5f;
	float isLockOnScale = lockOnScaleTmp;

	float lockOnAngle = 0.0f;

	//自然消滅フラグ
	bool isAnnihilation = false;

	//自キャラ
	Player* player_ = nullptr;


	ParticleManager* pManager;

	Vec2 posVec2;
	Vec2 moveVec;

	int count = 0;

	//ViewMat* view = nullptr;
	//ProjectionMat* projection = nullptr;

public:
	SoundData* soundData = nullptr;
	//int shotTime = 0;
	//ワールド変換データ
	WorldMat worldMat;

	const int lockCooltmp = 7;
	int lockCool = lockCooltmp;

	bool isLockOnDead = false;

	bool isGet = false;

	bool rightMove = false;

	//関数
private:


public:
	void Initialize(Player* player, const Vec3& pos, SoundData* soundData, ParticleManager* pManager/*, ViewMat* view, ProjectionMat* projection*/);
	~Item();

	void Update(ViewMat& view, ProjectionMat& projection);
	void Draw(ViewMat& view, ProjectionMat& projection, const UINT64* texHundle);

	Vec3 GetTrans();
	void MoveTrans(const Vec3& vec);

	void SetPlayer(Player* player) { player_ = player; }

	Vec3 GetWorldPos() override;

	void SetIsDead(const bool isDead) { this->isDead = isDead; }
	void SetIsAnnihilation(const bool isAnnihilation) { this->isAnnihilation = isAnnihilation; }

	void LockOnedReset();

	XMFLOAT4 GetColor();

	//衝突を検出したら呼び出す（コールバック関数）
	void OnCollision()override;
	/// <summary>
	/// playerにロックオンされたとき用
	/// </summary>
	void OnCollision2()override;

	//弾リストを取得(const参照)
	//const std::list<std::unique_ptr<EnemyBullet>>& GetBullets()
	//{
	//	return bullets_;
	//}

	bool IsDead() { return isDead; }
	/// <summary>
	/// 自然消滅したかどうかを返す
	/// </summary>
	bool IsAnnihilation() { return isAnnihilation; }
};

