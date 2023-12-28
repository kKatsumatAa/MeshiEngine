/*
* @file PlayerUIState.h
* @brief プレイヤーのレティクルの状態(プレイヤーの状態で変化)
*/

#pragma once
#include"Sprite.h"

//前方宣言
class PlayerUI;

class PlayerUIState
{
public:
	const Vec2 SPRITE_ANCOR_UV_ = { 0.5f,0.5f };
	const float TIME_RATIO_MAX_ = 1.0f;
	const float TIMER_MAX_ = 10;
	const float SCALE_MAX_ = 1.5f;
	const float SCALE_NORMAL_ = 1.0f;

protected:
	//
	PlayerUI* playerUI_ = nullptr;

	//スプライト用
	Sprite reticleSprite_;
	uint64_t reticleTexHandle_;

	//演出用
	float timer_ = 0;
	Vec4 color_ = { 1.0f,1.0f,1.0f,1.0f };
	float angle_ = 0;

public:
	//名前でステート取得
	static std::unique_ptr<PlayerUIState> GetState(const std::string& name);

public:
	//初期化
	virtual void Initialize();
	//更新
	virtual void Update();
	//スプライト描画
	virtual void DrawSprite();

public:
	//色をセット
	virtual void SetColor(const Vec4& color) { color_ = color; }
	//角度をセット
	virtual void SetAngle(float angle) { angle_ = angle; }

	//プレイヤーのUIのポインタをセット
	void SetPlayerUI(PlayerUI* playerUI) { playerUI_ = playerUI; }
};


//通常
class PlayerUIStateNomal : public PlayerUIState
{
public:
	//初期化
	virtual void Initialize();
	//更新
	virtual void Update();
	//スプライト描画
	virtual void DrawSprite();
};

//殴る
class PlayerUIStatePunch : public PlayerUIState
{
public:
	//初期化
	virtual void Initialize();
	//更新
	virtual void Update();
	//スプライト描画
	virtual void DrawSprite();
};

//銃
class PlayerUIStateGun : public PlayerUIState
{
public:
	//初期化
	virtual void Initialize();
	//更新
	virtual void Update();
	//スプライト描画
	virtual void DrawSprite();
};

//拾う
class PlayerUIStatePickUp : public PlayerUIState
{
public:
	//初期化
	virtual void Initialize();
	//更新
	virtual void Update();
	//スプライト描画
	virtual void DrawSprite();
};