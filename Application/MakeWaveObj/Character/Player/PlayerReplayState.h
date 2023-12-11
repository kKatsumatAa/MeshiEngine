#pragma once
#include"PlayerState.h"

class Player;


//リプレイ中かのステート
class PlayerReplayState
{
protected:
	Player* player_ = nullptr;

public:
	virtual ~PlayerReplayState() { ; }

	void SetPlayer(Player* player) { player_ = player; }

	virtual void Initialize();

	virtual void Unique() = 0;
};


//通常時のデータを保存する状態
class PlayerReplayStateSavingData : public PlayerReplayState
{
private:
	void Unique()override;
public:
	void Initialize()override;
};

//リプレイを再生してる状態
class PlayerReplayStateReplaying : public PlayerReplayState
{
private:
	void Unique()override;
public:
	void Initialize()override;
};