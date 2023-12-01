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

	Vec2 pos_;
	float scale_;
	float scale2_;

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
	void SetPos(const Vec2& pos) { pos_ = pos; }
	void SetScale(float scale) { scale_ = scale; }
	void SetScale2(float scale) { scale2_ = scale; }

	float GetScale() { return scale_; }
	float GetScale2() { return scale2_; }
	const Vec2& GetPos() { return pos_; }
};