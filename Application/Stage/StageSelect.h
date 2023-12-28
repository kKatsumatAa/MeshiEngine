/*
* @file StageSelect.h
* @brief ステージセレクトの見た目や処理
*/

#pragma once
#include"Util.h"
#include"DebugText.h"


class StageSelect final
{
private:
	const std::string BASE_DIRECTORY_ = "Resources/Level/levelData/";
	//ステージ名の拡大率
	const float TEXT_EXTEND_ = 1.5f;

public:
	const float STAGE_NAME_LEFT_UP_RATE_ = 4.0f;
	const Vec4 SELECT_BOX_COLOR_ = { 8.0f,0,0,1.0f };

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
	//カーソルがあっているときの範囲スプライト
	Sprite selectBox_;

	//デバッグテキスト
	DebugText debugText_;
	uint64_t debugTextHandle_;

private:
	//コンストラクタ
	StageSelect() { ; }
	//デストラクタ
	~StageSelect() { ; }

public:
	//コピーコンストラクタ禁止
	StageSelect& operator=(const StageSelect& inst) = delete;
	//コピーコンストラクタ禁止
	StageSelect(const StageSelect& inst) = delete;

	//インスタンス取得
	static StageSelect& GetInstance();

private:
	//ステージの名前読み込み
	void LoadStageNames();

public:
	//初期化
	void Initialize();
	//更新
	void Update();
	//描画
	void Draw();
	//スプライト描画
	void DrawSprite();

public:
	//ステージ選択済みか
	bool GetIsSelected() { return isSelected_; }
	//選択したステージの名前
	const std::string& GetSelectStageName() { return selectStageName_; }
	//選択したステージの番号
	int32_t GetSelectStageIndex() { return selectStageIndex_; }
};