/*
* @file PlayerUI.h
* @brief プレイヤーのレティクル処理
*/

#pragma once
#include"Object.h"

//プレイヤーのレティクルの状態の前方宣言
class PlayerUIState;

class PlayerUI final
{
public:
	enum RETICLENUM
	{
		NORMAL,
		PUNCH,
		GUN,
		PICK_UP,
	};

private:
	std::string oldStateName = "";

	Vec2 pos_;
	float scale_;
	float scaleRatio_;

	//ステート
	std::unique_ptr<PlayerUIState> state_ = nullptr;

private:
	//デストラクタ
	~PlayerUI() { ; }
	//コンストラクタ
	PlayerUI() { ; }

public:
	//コピーコンストラクタ禁止
	PlayerUI& operator=(const PlayerUI& inst) = delete;
	//コピーコンストラクタ禁止
	PlayerUI(const PlayerUI& inst) = delete;

	//インスタンス取得
	static PlayerUI& GetInstance();

public:
	//ステート変更
	void ChangeState(const std::string& name);

	//初期化
	void Initialize();
	//更新
	void Update();
	//スプライト描画
	void DrawSprite();

public:
	//角度セット
	void SetAngle(float angle);
	//位置セット
	void SetPos(const Vec2& pos) { pos_ = pos; }
	//スケールセット
	void SetScale(float scale) { scale_ = scale; }
	//スケール倍率セット
	void SetScaleRatio(float scale) { scaleRatio_ = scale; }

	//スケール取得
	float GetScale() { return scale_; }
	//スケール倍率取得
	float GetScaleRatio() { return scaleRatio_; }
	//位置取得
	const Vec2& GetPos() { return pos_; }
};