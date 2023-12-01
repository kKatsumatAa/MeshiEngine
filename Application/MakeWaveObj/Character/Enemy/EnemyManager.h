#pragma once
#include"Enemy.h"


class EnemyManager final
{
private:
	int32_t waveNum_ = 0;

private:
	EnemyManager() { ; }
	~EnemyManager() { ; }

public:
	EnemyManager& operator=(const EnemyManager& inst) = delete;
	EnemyManager(const EnemyManager& inst) = delete;

	static EnemyManager& GetInstance();

public:
	void Initialize();
	void Update();
	void Draw();

private:
	std::vector<IObject3D*>GetEnemys();
	Enemy* TransToEnemy(IObject3D* obj);

	//現在のウェーブ以外の敵の有効フラグをfalse
	void SetIsValidOtherWaveEnemy();
	//現在のウェーブの敵の有効フラグをtrue
	void SetIsValidWaveEnemy();
	//ウェーブの敵がもういないかどうか
	bool GetIsAliveWaveEnemy();
	//ウェーブ中の敵のクールタイムを減らす
	void UpdateWaveEnemyCoolTime();

public:
	//ゲームクリアかどうか
	bool GetIsAllEnemyDead();

public:
	static void DrawImGui();
};