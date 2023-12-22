/*
* @file Replay.h
* @brief リプレイするためのデータや再生
*/

#pragma once
#include"Vec2.h"
#include<vector>
#include<memory>
#include<functional>

//保存するデータ
struct ReplayData
{
	//キー（移動系）
	bool rightKey = false;
	bool leftKey = false;
	bool upKey = false;
	bool downKey = false;

	bool spaceKey = false;

	//マウス(移動量)
	Vec2 mouseCursorVel = { 0,0 };
	//(クリック)
	bool isLeftClickTrigger = false;
	bool isRightClickTrigger = false;

	//その時点のゲーム中のスピード
	float gameVel = 0.0f;
};


class Replay
{
private:
	//リプレイに必要な1フレームのデータの配列
	std::vector<std::unique_ptr<ReplayData>> rePlayDatas_;
	//現在フレームの通常時間での合計のデータ
	ReplayData normalTimeTotalReplayData_;

	int32_t replayDataCount_ = 0;
	//次のフレームに繰り越す速度
	float carryOverVel_ = 0;

public:
	void Initialize();
	//次のフレームの保存に移る
	void NextFrame();

	//合計で1.0fフレームになるように更新処理をする
	void UpdateWhile1Frame(std::function<void(const ReplayData& replay)>updateF);

	//時間を1.0に直したときの行動取得
	const ReplayData& GetNormalTimeTotalData();

public:
	//キー（移動系）
	void SetRightKey(bool rightKey) { rePlayDatas_.back()->rightKey = rightKey; }
	void SetLeftKey(bool leftKey) { rePlayDatas_.back()->leftKey = leftKey; }
	void SetUpKey(bool upKey) { rePlayDatas_.back()->upKey = upKey; }
	void SetDownKey(bool downKey) { rePlayDatas_.back()->downKey = downKey; }

	void SetSpaceKey(bool spaceKey) { rePlayDatas_.back()->spaceKey = spaceKey; }

	//マウス(移動量)
	void SetMouseCursorVel(const Vec2& cameraVel) { rePlayDatas_.back()->mouseCursorVel = cameraVel; }
	//(クリック)
	void SetIsLeftClickTrigger(bool isLeftClickTrigger) { rePlayDatas_.back()->isLeftClickTrigger = isLeftClickTrigger; }
	void SetIsRightClickTrigger(bool isRightClickTrigger) { rePlayDatas_.back()->isRightClickTrigger = isRightClickTrigger; }

	//その時点のゲーム中のスピード
	void SetGameVel(float gameVel) { rePlayDatas_.back()->gameVel = gameVel; }

public:
	//リプレイが終わりまで再生されたか
	bool GetReplayEnd() { return rePlayDatas_.size() <= replayDataCount_; }

public:
	//リプレイデータのカウント進める
	bool AddReplayDataCount();
	//リプレイデータのカウントリセット
	void ResetReplayDataCount() { replayDataCount_ = 0; }

};