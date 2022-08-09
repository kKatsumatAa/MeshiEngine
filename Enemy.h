#pragma once
#include "Assert.h"
#include"TimedCall.h"
#include"EnemyBullet.h"


//自機クラスの前方宣言(インクルードする代わりに)
class Player;

class Enemy;

class EnemyState
{
protected:
	Enemy* enemy;

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
	//ワールド変換データ
	WorldMat worldMat;
	//モデル
	Draw draw;
	//状態（行動）
	EnemyState* state;
	//弾
	std::list< std::unique_ptr<EnemyBullet>> bullets_;
	static const int shotCool = 60;
	std::list<std::unique_ptr<TimedCall>> timedCalls_;

	//自キャラ
	Player* player_ = nullptr;
public:
	//int shotTime = 0;


	//関数
private:


public:
	Enemy(Player* player);
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

	//衝突を検出したら呼び出す（コールバック関数）
	void OnCollision()override;

	//弾リストを取得(const参照)
	const std::list<std::unique_ptr<EnemyBullet>>& GetBullets()
	{
		return bullets_;
	}
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


const Vec3 approachSpeed = { 0,0,-0.2 };
const Vec3 leaveSpeed = { -0.2,0.2,0 };