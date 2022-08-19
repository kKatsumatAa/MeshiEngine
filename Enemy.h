#pragma once
#include "Assert.h"
#include"TimedCall.h"
#include"EnemyBullet.h"
#include"BulletManager.h"


//自機クラスの前方宣言(インクルードする代わりに)
class Player;

class Enemy;

class EnemyState
{
protected:
	Enemy* enemy = nullptr;

public:
	void SetEnemy(Enemy* enemy);
	virtual void Update() = 0;
};

///<summary>
///敵
///</summary>
class Enemy : public Collider
{
	//変数
private:
	
	//モデル
	Draw draw;
	Draw lockOn;
	Draw lockOnLine;

	float lockOnLength = 0;
	Vec3 lockOnVec;
	bool isLockOnDead = false;

	const float lockOnScaleTmp = 1.5f;
	float isLockOnScale = lockOnScaleTmp;

	float lockOnAngle = 0.0f;

	//自然消滅フラグ
	bool isAnnihilation = false;

	//状態（行動）
	EnemyState* state;
	////弾
	//std::list< std::unique_ptr<EnemyBullet>> bullets_;
	static const int shotCool = 180;
	std::list<std::unique_ptr<TimedCall>> timedCalls_;

	//自キャラ
	Player* player_ = nullptr;

	//弾マネージャー
	BulletManager* bulletManager = nullptr;

	

public:
	bool isBoss = false;
	//int shotTime = 0;
	//ワールド変換データ
	WorldMat worldMat;

	const int lockCooltmp = 7;
	int lockCool = lockCooltmp;

	//関数
private:


public:
	/*Enemy(Player* player);*/
	void Initialize(Player* player, BulletManager* bulletManager,const Vec3& pos);
	~Enemy();
	void InitializeApproach();

	void Update();
	void Fire();
	void ChangeState(EnemyState* state);
	/// <summary>
	/// 弾発射、タイマーリセット
	/// </summary>
	/// <param name="view"></param>
	void ShotResetTimer();
	void Draw(ViewMat& view,ProjectionMat& projection, const UINT64* texHundle);

	Vec3 GetTrans();
	void MoveTrans(const Vec3& vec);

	void RemoveTimeCall();

	void SetPlayer(Player* player) { player_ = player; }

	Vec3 GetWorldPos() override;

	void SetIsDead(const bool isDead) { this->isDead = isDead; }
	void SetIsAnnihilation(const bool isAnnihilation) { this->isAnnihilation = isAnnihilation; }

	void LockOnedReset();

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

class EnemyStateApproach :public EnemyState
{
public:
	void Update();
};

class EnemyStateLeave :public EnemyState
{
public:
	void Update();
};


const Vec3 approachSpeed = { 0,0,-0.2f };
const Vec3 leaveSpeed = { 0.0f,0.8f,0 };