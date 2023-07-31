#pragma once
#include<string>

class GameVelocityManager final
{
private:
	float gameVelocity_ = 0;
	//�Q�[���X�s�[�h�̉��Z����
	bool isInvalidAddGameVel_ = false;

public:
	const float GAME_VELOCITY_MAX_ = 1.0f;
	const float GAME_VELOCITY_MIN_ = 0.03f;

	//�}�E�X�݂̂̍ő呬�x
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