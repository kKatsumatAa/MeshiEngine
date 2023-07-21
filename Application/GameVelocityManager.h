#pragma once
#include<string>

class GameVelocityManager final
{
private:
	float gameVelocity_ = 0;

public:
	const float GAME_VELOCITY_MAX_ = 1.0f;
	const float GAME_VELOCITY_MIN_ = 0.03f;

	//演出用の画面の最低サイズ
	const float DISPLAY_SIZE_MIN_ = 0.9f;

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
};