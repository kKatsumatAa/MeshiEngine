/*
* @file PlayerReplayState.h
* @brief プレイヤーのリプレイ状態（保存か再生しているか）
*/

#pragma once
#include"PlayerState.h"

class Player;


//リプレイ中かのステート
class PlayerReplayState
{
protected:
	Player* player_ = nullptr;

public:
	//デストラクタ
	virtual ~PlayerReplayState() { ; }

	//プレイヤーポインタセット
	void SetPlayer(Player* player) { player_ = player; }
	
	//初期化
	virtual void Initialize();
	//更新
	virtual void Unique() = 0;
};


//通常時のデータを保存する状態
class PlayerReplayStateSavingData : public PlayerReplayState
{
private:
	//ステート固有の処理
	void Unique()override;
public:
	//初期化
	void Initialize()override;
};

//リプレイを再生してる状態
class PlayerReplayStateReplaying : public PlayerReplayState
{
private:
	//ステート固有の処理
	void Unique()override;
public:
	//初期化
	void Initialize()override;
};