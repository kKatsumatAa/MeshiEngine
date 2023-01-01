#pragma once
#include"PlayerBullet.h"
#include"ItemManager.h"
#include"CartridgeEffectManager.h"
#include"Camera.h"

class PlayerBulletManager;


class PlayerBulletState
{
protected:
	PlayerBulletManager* playerBulletM;

public:
	void SetPlayerBulletManager(PlayerBulletManager* playerBulletM);
	virtual void Shot(Vec3 pos, std::function<void()> p) = 0;
};

//普通の弾
class BulletNormal : public PlayerBulletState
{
private:
	const int coolTimeTmp = 10;
	int coolTime = 0;


public:
	void Shot(Vec3 pos, std::function<void()> p) override;
};

//レーザー
class BulletLayser : public PlayerBulletState
{
private:


public:
	void Shot(Vec3 pos, std::function<void()> p) override;
};

//ショットガン
class BulletShotGun : public PlayerBulletState
{
private:
	const int coolTimeTmp = 20;
	int coolTime = 0;
	const int num = 5;

public:
	void Shot(Vec3 pos, std::function<void()> p) override;
};


//---------------------------------------------------------------------------------------
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

	PlayerBulletState* state = nullptr;

	bool isChange = false;

	Camera* camera;

	const float shakeLength = 1.4f;
	const int shakeTime = 7;

public:
	//薬莢エフェクト
	CartridgeEffectManager* cartridgeEffectM;

	//敵の弾
	std::list< std::unique_ptr<PlayerBullet>> playerBullets_;


	void Initialize(Model* model, CartridgeEffectManager* cartridgeEffectManager, Camera* camera);

	void ChangeState(PlayerBulletState* state);

	int GetBulletNum() { return bulletNum; }
	int GetBulletNumMax() { return bulletNumMax; }
	void SetBulletNum(int num);
	//void SetBulletNumMax(int num) { bulletNumMax = num; }

	//弾の種類
	int GetBulletType() { return bulletType; }
	void SetBulletType(const int bulletType);

	void GeneratePlayerBullet(const Vec3& position, const Vec3& velocity);
	void Update();
	void Draw(ViewMat& view, ProjectionMat& projection);
	void DrawSprite();

	void ShakeCamera();

	//弾打つ
	void Shot(Vec3 pos, std::function<void()> p);

	//弾リストを取得(const参照)
	const std::list<std::unique_ptr<PlayerBullet>>& GetPlayerBullets()
	{
		return playerBullets_;
	}
};

