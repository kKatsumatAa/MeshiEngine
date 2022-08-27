#pragma once
#include"PlayerBullet.h"
#include"Sound.h"

enum
{
	NORMAL,
	TARGET
};

class Player : public Collider
{
private:
	//ワールド変換データ
	WorldMat worldMat;
	//モデル
	Draw draw;
	std::list< std::unique_ptr<PlayerBullet>> bullets_;
	static const int shotCool = 10;
	int shotTime = 0;
	int status = NORMAL;
	SoundData* shotSE = nullptr;
	const int coolTimetmp = 120;
    float coolColor = 1.0f;

public:
	void Initialize(SoundData* shotSE);
	Player();
	void Attack();
	void Update();
	void Draw(ViewMat& view, ProjectionMat& projection, const UINT64* texHundle, const UINT64* numTexHundle);
	int  GetPlayerStatus() { return status; }

	Vec3 GetWorldPos() override;

	//衝突を検出したら呼び出す（コールバック関数）
	void OnCollision() override;
	//敵ロックオン時にする処理
	void OnCollision2()override;

	//弾リストを取得
	const std::list<std::unique_ptr<PlayerBullet>>& GetBullets()
	{
		return bullets_;
	}

};

static const XMFLOAT2 playerMoveRange = { 63.0f,65.0f / 2.0f };
