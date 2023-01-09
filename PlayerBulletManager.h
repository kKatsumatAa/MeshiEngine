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

//���ʂ̒e
class BulletNormal : public PlayerBulletState
{
private:
	const int coolTimeTmp = 8;
	int coolTime = 0;


public:
	void Shot(Vec3 pos, std::function<void()> p) override;
};

//���[�U�[
class BulletLayser : public PlayerBulletState
{
private:
	bool isShot = false;
	int time = 0;
	const int timerTmp = 1;
	int count = 0;
	const int countTmp = 50;

public:
	void Shot(Vec3 pos, std::function<void()> p) override;
};

//�V���b�g�K��
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

	static const int bulletNumMax = 8;
	int bulletNum = bulletNumMax;

	Object gauge;
	Object amoGauge[bulletNumMax];


	UINT64 texhandle[10] = { NULL };

	int bulletType = ITEM_TYPE::INORMAL;

	PlayerBulletState* state = nullptr;

	bool isChange = false;

	Camera* camera;

	const float shakeLength = 1.1f;
	const int shakeTime = 7;

public:
	//��䰃G�t�F�N�g
	CartridgeEffectManager* cartridgeEffectM;

	//�G�̒e
	std::list< std::unique_ptr<PlayerBullet>> playerBullets_;


	void Initialize(Model* model, CartridgeEffectManager* cartridgeEffectManager, Camera* camera);

	void ChangeState(PlayerBulletState* state);

	int GetBulletNum() { return bulletNum; }
	int GetBulletNumMax() { return bulletNumMax; }
	void SetBulletNum(int num);
	//void SetBulletNumMax(int num) { bulletNumMax = num; }

	//�e�̎��
	int GetBulletType() { return bulletType; }
	void SetBulletType(const int bulletType);

	void GeneratePlayerBullet(const Vec3& position, const Vec3& velocity);
	void Update();
	void Draw(ViewMat& view, ProjectionMat& projection);
	void DrawSprite();

	void ShakeCamera();

	//�e�ł�
	void Shot(Vec3 pos, std::function<void()> p);

	//�e���X�g���擾(const�Q��)
	const std::list<std::unique_ptr<PlayerBullet>>& GetPlayerBullets()
	{
		return playerBullets_;
	}
};

