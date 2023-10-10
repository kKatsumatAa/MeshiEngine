#pragma once
#include"Object.h"


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

	//ステート
	std::unique_ptr<PlayerUIState> state_ = nullptr;

private:
	~PlayerUI() { ; }
	PlayerUI() { ; }

public:
	PlayerUI& operator=(const PlayerUI& inst) = delete;
	PlayerUI(const PlayerUI& inst) = delete;

	static PlayerUI& GetInstance();

public:
	void ChangeState(const std::string& name);

	void Initialize();
	void Update();
	void DrawSprite();

public:
	void SetAngle(float angle);
};