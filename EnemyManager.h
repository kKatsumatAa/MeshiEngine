#pragma once
#include"Enemy.h"
#include"Particle.h"
#include"ItemManager.h"
#include<sstream>

class EnemyManager
{
private:
	SoundData* soundData;
	Player* player;
	BulletManager* bulletManager;
	//敵発生コマンド
	std::stringstream enemyPopCommands;
	//待機
	bool isWait = false;
	int  waitTimer = 0;

	ParticleManager* particleManager;
	ItemManager* itemManager;

	int infoTimer = 0;
	bool oldInfo = false;


	/// <summary>
	/// 敵発生コマンドの更新
	/// </summary>
	void UpdateEnemyPopCommands();
	void EnemyGenerate(const Vec3& pos);
	void BossGenerate(const Vec3& pos, float& scale, int& HP);
	void Enemy2Generate(const Vec3& pos);
	

public:
	std::list<std::unique_ptr<Enemy>> enemies;
	int phase = 0;
	//phaseが変わるまで待つフラグ
	bool isPhase = false;
	bool isEnd[2] = { false };
	bool isItem = false;
	bool isBossDead = false;


	void Initialize(Player* player, BulletManager* bulletManager,SoundData* soundData, ParticleManager* pManager,ItemManager* iManager);
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

	void StartGenerate();

	void InfoEnd(bool& infoEnd);
};

