#include "PlayerReplayState.h"
#include "Player.h"
#include "Replay.h"
#include "GameVelocityManager.h"


void PlayerReplayState::Initialize()
{
	//ステートで行うプレイヤーの処理
	player_->SetReplayStateF([=]()
		{
			Unique();
		}
	);
}


//---------------------------------------------------------------------------------
//通常時のデータを貯めてる状態

void PlayerReplayStateSavingData::Unique()
{
	//ゲームのスピード保存
	player_->GetReplay()->SetGameVel(GameVelocityManager::GetInstance().GetVelocity());

	//カメラの向き変更
	player_->DirectionUpdate();

	//移動
	player_->Move();

	//リプレイの保存フレームを次に
	player_->GetReplay()->NextFrame();
}

void PlayerReplayStateSavingData::Initialize()
{
	//リプレイの保存フレームを次に
	player_->GetReplay()->NextFrame();

	PlayerReplayState::Initialize();
}


//--------------------------------------------------------
//リプレイ中

void PlayerReplayStateReplaying::Unique()
{
	//リプレイデータ
	Replay* replay = player_->GetReplay();

	ReplayData data = replay->GetNormalTimeTotalData();

	//クリック
	player_->SetIsClickLeft(data.isLeftClickTrigger);
	player_->SetIsClickRight(data.isRightClickTrigger);

	//リプレイデータのベクトルなどを使う
	player_->DirectionUpdateCalcPart(data.mouseCursorVel);
	player_->MoveCalcPart(data.leftKey, data.rightKey, data.upKey, data.downKey, data.spaceKey,
		data.gameVel);

	//データ要素番号を次へ
	replay->AddReplayDataCount();
}

void PlayerReplayStateReplaying::Initialize()
{


	PlayerReplayState::Initialize();
}