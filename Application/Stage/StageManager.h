/*
* @file StageManager.h
* @brief ステージ内のものを管理（ステージ読み込んだりクリアしたかとか）
*/

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
#include "Replay.h"


class StageState;

class StageManager final
{
public:
	//海
	const float SEA_DICTANCE_TMP_ = 120.0f;
	const float LAVA_APPROACH_TIME_ = 20.0f * 60.0f / SEA_DICTANCE_TMP_;
	const float POST_SEA_TIME_EXTEND_RATE_ = 1.2f;
	const int32_t SEA_MAX_AFTER_TIME_ = 60;
	const float SEA_DISTANCE_RATIO_MAX_ = 1.0f;
	const float SEA_AFTER_TIME_RATIO_MAX_ = 1.0f;
	//海の時間で地形を削除する時間の割合
	const float ERASE_LAND_SHAPE_SEA_TIME_RATIO_ = 0.8f;
	const float IMGUI_SEA_DIST_DRAG_SPEED_ = 0.5f;
private:
	int32_t stageNum_ = 1;
	std::unique_ptr<StageState> state_ = nullptr;

	//ステージクリアしたか
	bool isClear_ = false;
	bool isGameOver_ = false;

	//リプレイ
	std::unique_ptr<Replay> replay_ = nullptr;

private:
	Vec3 seaDistance_ = { 0,SEA_DICTANCE_TMP_,0 };
	//溶岩が最大まで来た後の残り時間
	int32_t seaMaxAfterTime_ = SEA_MAX_AFTER_TIME_;

private:
	//コンストラクタ
	StageManager() { ; }
	//デストラクタ
	~StageManager() { ; }

public:
	//コピーコンストラクタ禁止
	StageManager& operator=(const StageManager& inst) = delete;
	//コピーコンストラクタ禁止
	StageManager(const StageManager& inst) = delete;

	//インスタンス取得
	static StageManager& GetInstance();

private:
	//初期化
	void Initialize(const std::string& stateName);
public:
	//リプレイ初期化
	void InitializeReplay();

public:
	//ステージ読み込み
	void LoadStage(int32_t stageIndex, const std::string& stateName = "BEGINING");
	//ステージ読み込み(現在のステージを再読み込み)
	void LoadStage(const std::string& stateName = "BEGINING");

	//ステージ番号セット
	void SetStageNum(int32_t stageNum) { stageNum_ = stageNum; }
	//ステージ番号取得
	int32_t GetStageNum() { return stageNum_; }

	//ステージクリアしたかセット
	void SetIsClear(bool isClear) { isClear_ = isClear; }
	//ステージクリアしたか取得
	bool GetIsGameClear() { return isClear_; }

	//ステージゲームオーバーかセット
	void SetIsGameOver(bool isGameOver) { isGameOver_ = isGameOver; }
	//ステージゲームオーバーか取得
	bool GetIsGameOver() { return isGameOver_; }

	//リプレイ取得
	Replay* GetReplay() { return replay_.get(); }

public:
	//海までの距離取得
	const Vec3& GetSeaDistance() { return seaDistance_; }
	//海までの距離セット
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
	//更新
	void Update();
	//影用の深度描画
	void DrawShadow();
	//描画
	void Draw();
	//ImGuiの描画
	void DrawImGui();
	//スプライト描画
	void DrawSprite();
};