#pragma once
#include"Sprite.h"


class ClearEffectState;

class ClearEffect final
{
private:
	//ステート
	std::unique_ptr<ClearEffectState> state_ = nullptr;

	bool isEffect_ = false;

private:
	ClearEffect() { ; }
	~ClearEffect() { ; }

public:
	void SetIsEffect(bool isEffect) { isEffect_ = isEffect; }
	bool GetIsEffect() { return isEffect_; }


public:
	ClearEffect& operator=(const ClearEffect& obj) = delete;
	ClearEffect(const ClearEffect& obj) = delete;

	static ClearEffect& GetInstance();

	void ChangeState(std::unique_ptr<ClearEffectState> state);
	
	void BeginClearEffect();

	void Initialize();
	void Update();
	void Draw();

};