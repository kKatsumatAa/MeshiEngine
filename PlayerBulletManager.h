#pragma once
#include"PlayerBullet.h"
#include"ItemManager.h"

class PlayerBulletManager
{
private:
	Model* model = nullptr;
	//sound
	Sound* audio = nullptr;

	const int bulletNumMax = 8;
	int bulletNum = bulletNumMax;

	Object gauge[2];

	UINT64 texhandle = NULL;

	int bulletType = ITEM_TYPE::INORMAL;

public:
	//敵の弾
	std::list< std::unique_ptr<PlayerBullet>> playerBullets_;


	void Initialize(Model* model);

	int GetBulletNum() { return bulletNum; }
	int GetBulletNumMax() { return bulletNumMax; }
	void SetBulletNum(int num) { bulletNum = num; }
	//void SetBulletNumMax(int num) { bulletNumMax = num; }

	//弾の種類
	int GetBulletType() { return bulletType; }
	void SetBulletType(const int bulletType) { this->bulletType = bulletType; }

	void GeneratePlayerBullet(const Vec3& position, const Vec3& velocity);
	void Update();
	void Draw(ViewMat& view, ProjectionMat& projection);
	void DrawSprite();

	//弾リストを取得(const参照)
	const std::list<std::unique_ptr<PlayerBullet>>& GetPlayerBullets()
	{
		return playerBullets_;
	}
};

