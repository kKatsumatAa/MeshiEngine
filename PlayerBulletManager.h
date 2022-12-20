#pragma once
#include"PlayerBullet.h"

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

public:
	//“G‚Ì’e
	std::list< std::unique_ptr<PlayerBullet>> playerBullets_;


	void Initialize(Model* model);

	int GetBulletNum() { return bulletNum; }
	int GetBulletNumMax() { return bulletNumMax; }
	void SetBulletNum(int num) { bulletNum = num; }
	//void SetBulletNumMax(int num) { bulletNumMax = num; }

	void GeneratePlayerBullet(const Vec3& position, const Vec3& velocity);
	void Update();
	void Draw(ViewMat& view, ProjectionMat& projection);
	void DrawSprite();

	//’eƒŠƒXƒg‚ğæ“¾(constQÆ)
	const std::list<std::unique_ptr<PlayerBullet>>& GetPlayerBullets()
	{
		return playerBullets_;
	}
};

