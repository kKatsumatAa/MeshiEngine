/*
* @file GameVelocityManager.h
* @brief ゲーム全体のスピードを管理
*/

#pragma once
#include<string>
#include"Vec3.h"
#include"Vec4.h"

class GameVelocityManager final
{
public:
	//ゆっくりな時の色
	const Vec3 VEL_COLOR_MIN_ = { 0.4f,0.7f,0.9f };
	const Vec4 VEL_COLOR_MAX_ = { 1.0f,1.0f,1.0f,1.0f };

	const float VEL_RADIAL_BLUR_RATE_ = 0.6f;
	const float GAME_VEL_DECTREMENT_ = 0.95f;

private:
	float gameVelocity_ = 0;
	//ゲームスピードの加算無効
	bool isInvalidAddGameVel_ = false;
	//1.0f固定になる
	bool isNormalTime_ = false;

public:
	const float GAME_VELOCITY_MAX_ = 1.0f;
	const float GAME_VELOCITY_MIN_ = 0.03f;

	//マウスのみの最大速度
	const float MOUSE_VEL_MAX_ = GAME_VELOCITY_MAX_ / 7.0f;

private:
	//コンストラクタ
	GameVelocityManager() { ; }
	//デストラクタ
	~GameVelocityManager() { ; }

public:
	//コピーコンストラクタ禁止
	GameVelocityManager(const GameVelocityManager& other) = delete;
	//コピーコンストラクタ禁止
	GameVelocityManager& operator=(const GameVelocityManager& other) = delete;

	//インスタンス取得
	static GameVelocityManager& GetInstance();

public:
	//初期化
	void Initialize();
	//更新
	void Update();

public:
	//ゲームスピードを加算
	void AddGameVelocity(float velocity, std::string velName = "NULL");
	//ゲームスピード取得
	float GetVelocity();

public:
	//ImGuiの描画
	void UpdateImGui();

public:
	//ゲームスピードの加算無効をセット
	void SetIsInvalidAddGameVel(bool is) { isInvalidAddGameVel_ = is; }
	//ゲームスピードの加算無効を取得
	bool GetIsInvalidAddGameVel() { return isInvalidAddGameVel_; }
	//ゲームスピード1.0固定をセット
	void SetIsNormalTime(bool is) { isNormalTime_ = is; }
	//ゲームスピード1.0固定を取得
	bool GetIsNormalTime() { return isNormalTime_; }
};