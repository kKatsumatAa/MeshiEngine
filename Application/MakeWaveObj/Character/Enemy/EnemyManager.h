/*
* @file EnemyManager.h
* @brief 敵のウェーブなどを管理し、出現させたり（更新などはObjectManagerの方で）
*/

#pragma once
#include"Enemy.h"


class EnemyManager final
{
private:
	int32_t waveNum_ = 0;

private:
	//コンストラクタ
	EnemyManager() { ; }
	//デストラクタ
	~EnemyManager() { ; }

public:
	//コピーコンストラクタ禁止
	EnemyManager& operator=(const EnemyManager& inst) = delete;
	//コピーコンストラクタ禁止
	EnemyManager(const EnemyManager& inst) = delete;

	//インスタンス取得
	static EnemyManager& GetInstance();

public:
	//初期化
	void Initialize();
	//更新
	void Update();
	//描画
	void Draw();

private:
	//敵の配列取得
	std::vector<IObject3D*>GetEnemys();
	//IObject3DをEnemyのポインタに変換
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
	//ImGui描画
	static void DrawImGui();
};