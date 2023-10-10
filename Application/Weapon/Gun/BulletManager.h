#pragma once
#include"Bullet.h"

class BulletManager final
{
private:
	//弾の配列
	std::list<std::unique_ptr<Bullet>> bullets_;


private:
	BulletManager() { ; }
	~BulletManager() { ; }

public:
	BulletManager& operator=(const BulletManager& obj) = delete;
	BulletManager(const BulletManager& obj) = delete;

	static BulletManager& GetInstance();

public:
	void Initialize();

	void CreateBullet(const Vec3& pos, const Vec3& directionVec, float scale, float lifeTime, IObject* owner);

	void Update();

	void Draw();
};