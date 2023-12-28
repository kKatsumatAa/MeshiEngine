#include "StageSelect.h"
#include "MouseInput.h"
#include "CursorUI.h"


StageSelect& StageSelect::GetInstance()
{
	static StageSelect sInst;
	return sInst;
}

void StageSelect::LoadStageNames()
{
	GetFileNames(BASE_DIRECTORY_, stageNames_);
}

//----------------------------------------------------------
void StageSelect::Initialize()
{
	//画像
	debugTextHandle_ = TextureManager::LoadGraph("ascii.png");

	//リセット
	stageNames_.clear();
	isSelected_ = false;

	//ステージ名の枠の大きさ
	nameWidthHeight_ = { CursorUI::cursorSize_.x * nameFrameWidthExtend_ ,CursorUI::cursorSize_.y };
	namesLeftUpPos_ = { CursorUI::cursorSize_.x * nameFrameWidthExtend_ ,CursorUI::cursorSize_.y * STAGE_NAME_LEFT_UP_RATE_ };

	//色
	selectBox_.SetColor(SELECT_BOX_COLOR_);

	LoadStageNames();
}

void StageSelect::Update()
{
	//スクロール処理


	//クリックされたら
	if (MouseInput::GetInstance().GetTriggerClick(CLICK_LEFT))
	{
		Vec2 cursorPos = CursorUI::GetInstance().GetCursorPos();

		//当たり判定を調べる
		int32_t count = 0;
		for (auto stageName : stageNames_)
		{
			//当たっていれば選んだ名前,番号を保存
			if (CollisionBox(namesLeftUpPos_ + Vec2{ 0, nameWidthHeight_.y * (float)count },//左上
				namesLeftUpPos_ + Vec2{ nameWidthHeight_.x, nameWidthHeight_.y * (float)(count + 1) },//右下
				cursorPos + Vec2{ 0, scrollValue_ },
				cursorPos + Vec2{ 0, scrollValue_ }))
			{
				selectStageName_ = stageName;
				selectStageIndex_ = count + 1;//countは0から始まるので1からにする
				isSelected_ = true;

				break;
			}

			count++;
		}
	}
}

void StageSelect::Draw()
{
}

void StageSelect::DrawSprite()
{
	int32_t count = 0;

	for (auto stageName : stageNames_)
	{
		//ステージ名表示
		debugText_.Print(stageName.c_str(), namesLeftUpPos_.x, namesLeftUpPos_.y + nameWidthHeight_.y * count, DebugText::S_INVALID_TMP_,
			TEXT_EXTEND_);

		Vec2 cursorPos = MouseInput::GetInstance().GetCurcorPos();
		//カーソルがあってたら赤く表示
		if (CollisionBox(namesLeftUpPos_ + Vec2{ 0, nameWidthHeight_.y * (float)count },//左上
			namesLeftUpPos_ + Vec2{ nameWidthHeight_.x, nameWidthHeight_.y * (float)(count + 1) },//右下
			cursorPos + Vec2{ 0, scrollValue_ },
			cursorPos + Vec2{ 0, scrollValue_ }))
		{
			selectBox_.SetTrans({ namesLeftUpPos_ + Vec2{0, nameWidthHeight_.y} *(float)count,0 });
			selectBox_.SetScale({ nameWidthHeight_,1.0f });

			selectBox_.DrawBoxSprite(nullptr);
		}

		count++;
	}

	debugText_.DrawAll(debugTextHandle_, nullptr);
}
