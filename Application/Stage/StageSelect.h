#pragma once
#include"Util.h"
#include"DebugText.h"


class StageSelect final
{
private:
	const std::string BASE_DIRECTORY_ = "Resources/Level/levelData/";

private:
	const float nameFrameWidthExtend_ = 20.0f;
	//選択済みか
	bool isSelected_ = false;
	//選択したステージ名
	std::string selectStageName_;
	int32_t selectStageIndex_ = 0;
	//セレクト一つの幅
	Vec2 nameWidthHeight_;
	//セレクトまとまりの左上位置
	 Vec2 namesLeftUpPos_;
	//ステージがどれだけあるか読み込み、ためておく
	std::vector<std::string> stageNames_;
	//どれだけスクロールされたか
	float scrollValue_ = 0;
	//
	Object selectBox_;

	//デバッグテキスト
	DebugText debugText_;
	uint64_t debugTextHandle_;

private:
	StageSelect() { ; }
	~StageSelect() { ; }

public:
	StageSelect& operator=(const StageSelect& inst) = delete;
	StageSelect(const StageSelect& inst) = delete;

	static StageSelect& GetInstance();

private:
	//ステージの名前読み込み
	void LoadStageNames();

public:
	void Initialize(
	);
	void Update();
	void Draw();
	void DrawSprite();

public:
	bool GetIsSelected() { return isSelected_; }
	const std::string& GetSelectStageName() { return selectStageName_; }
	int32_t GetSelectStageIndex() { return selectStageIndex_; }

};