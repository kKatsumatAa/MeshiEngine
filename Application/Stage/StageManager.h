#pragma once
#include"LevelManager.h"
#include "JsonLevelLoader.h"
#include "CameraManager.h"
#include "CollisionManager.h"
#include "GameVelocityManager.h"
#include "BulletManager.h"
#include "ClearEffect.h"
#include "ClearEffectState.h"
#include "TutorialState.h"
#include "Tutorial.h"
#include "ParticleManager.h"
#include "PlayerUI.h"
#include "PlayerUIState.h"
#include "EnemyManager.h"


class StageState;

class StageManager final
{
private:
	int32_t stageNum_ = 1;
	std::unique_ptr<StageState> state_ = nullptr;

	//ステージクリアしたか
	bool isClear_ = false;
	bool isGameOver_ = false;

public:
	//海からの初期距離
	const float SEA_DICTANCE_TMP_ = 120.0f;
	const float LAVA_APPROACH_TIME_ = 20.0f * 60.0f / SEA_DICTANCE_TMP_;
private:
	Vec3 seaDistance_ = { 0,SEA_DICTANCE_TMP_,0 };
	//溶岩が最大まで来た後の残り時間
	const int32_t SEA_MAX_AFTER_TIME_ = 60;
	int32_t seaMaxAfterTime_ = SEA_MAX_AFTER_TIME_;

public:


private:
	StageManager() { ; }
	~StageManager() { ; }

public:
	StageManager& operator=(const StageManager& inst) = delete;
	StageManager(const StageManager& inst) = delete;

	static StageManager& GetInstance();

private:
	void Initialize();

public:
	//ステージ読み込み
	void LoadStage(int32_t stageIndex);

	//ステージ番号
	void SetStageNum(int32_t stageNum) { stageNum_ = stageNum; }
	int32_t GetStageNum() { return stageNum_; }

	//ステージクリアしたか
	void SetIsClear(bool isClear) { isClear_ = isClear; }
	bool GetIsGameClear() { return isClear_; }

	//ステージゲームオーバー
	void SetIsGameOver(bool isGameOver) { isGameOver_ = isGameOver; }
	bool GetIsGameOver() { return isGameOver_; }

public:
	const Vec3& GetSeaDistance() { return seaDistance_; }
	void SetSeaDistance(const Vec3& seaDistance) { seaDistance_ = seaDistance; }

public:
	//溶岩が迫る処理
	void ApproachLava();
	//溶岩が最大まで来てその後のカウントダウン
	void AfterLavaMaxUpdate();

public:
	//ステート変更
	void ChangeState(const std::string& name);
	
public:
	void Update();
	void DrawShadow();
	void Draw();
	void DrawImGui();
	void DrawSprite();
};