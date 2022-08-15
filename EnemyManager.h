#pragma once
#include"Enemy.h"
#include<sstream>

class EnemyManager
{
private:
	SoundData* soundData;
	Player* player;
	BulletManager* bulletManager;

	float gTimer = 300;
	//敵発生コマンド
	std::stringstream enemyPopCommands;
	//待機
	bool isWait = false;
	int  waitTimer = 0;


	/// <summary>
	/// 敵発生コマンドの更新
	/// </summary>
	void UpdateEnemyPopCommands();
	void EnemyGenerate(const Vec3& pos);

public:
	std::list<std::unique_ptr<Enemy>> enemies;


	void Initialize(Player* player, BulletManager* bulletManager,SoundData* soundData);
	void Update();
	void Draw(ViewMat& view, ProjectionMat& projection, const UINT64* texHundle);

	//弾リストを取得(const参照)
	const std::list<std::unique_ptr<Enemy>>& GetEnemies()
	{
		return enemies;
	}

	/// <summary>
	/// 敵発生データの読み込み
	/// </summary>
	void LoadEnemyPopData();

	
};

