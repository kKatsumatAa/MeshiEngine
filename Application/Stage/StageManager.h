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
	//ステート変更
	void ChangeState(const std::string& name);
	
public:
	void Update();
	void Draw();
	void DrawImGui();
	void DrawSprite();
};