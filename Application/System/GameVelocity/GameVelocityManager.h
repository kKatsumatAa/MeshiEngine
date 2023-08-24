#pragma once
#include<string>
#include"Vec3.h"

class GameVelocityManager final
{
private:
	float gameVelocity_ = 0;
	//ゲームスピードの加算無効
	bool isInvalidAddGameVel_ = false;
	//色
	const Vec3 VEL_COL_MIN_ = { 0.5f,0.8f,0.8f };

public:
	const float GAME_VELOCITY_MAX_ = 1.0f;
	const float GAME_VELOCITY_MIN_ = 0.03f;

	//マウスのみの最大速度
	const float MOUSE_VEL_MAX_ = GAME_VELOCITY_MAX_ / 7.0f;

private:
	GameVelocityManager() { ; }
	~GameVelocityManager() { ; }

public:
	GameVelocityManager(const GameVelocityManager& other) = delete;
	GameVelocityManager& operator=(const GameVelocityManager& other) = delete;

	static GameVelocityManager& GetInstance();

public:
	void Initialize();

	void Update();

	void AddGameVelocity(float velocity, std::string velName = "NULL");

	float GetVelocity();

	void UpdateImGui();

public:
	void SetIsInvalidAddGameVel(bool is) { isInvalidAddGameVel_ = is; }
	bool GetIsInvalidAddGameVel() { return isInvalidAddGameVel_; }
};