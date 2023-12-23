/*
* @file BulletManager.h
* @brief 銃の弾丸を一気に管理（更新、削除等）
*/

#pragma once
#include"Bullet.h"

class BulletManager final
{
private:
	//弾の配列
	std::list<std::unique_ptr<Bullet>> bullets_;


private:
	//コンストラクタ
	BulletManager() { ; }
	//デストラクタ
	~BulletManager() { ; }

public:
	//コピーコンストラクタ禁止
	BulletManager& operator=(const BulletManager& obj) = delete;
	//コピーコンストラクタ禁止
	BulletManager(const BulletManager& obj) = delete;

	//インスタンス取得
	static BulletManager& GetInstance();

public:
	//初期化
	void Initialize();
	//更新
	void Update();
	//影用の深度描画
	void DrawShadow();
	//描画
	void Draw();

public:
	//弾丸の生成
	void CreateBullet(const Vec3& pos, const Vec3& directionVec, float scale, float lifeTime, IObject* owner);
};