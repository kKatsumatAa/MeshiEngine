#pragma once
#include"Util.h"
#include"Sprite.h"
#include<functional>


class ClearEffect;

class ClearEffectState
{
protected:
	//状態を使うインスタンスポインタ
	ClearEffect* clearEffect_ = nullptr;

	int32_t timer_ = 0;
	const int32_t TIMER_MAX_ = 70;
	float t_ = 0;

	float scale_ = 0;
	const float MAX_SCALE_ = 1.5f;

	float alpha_ = 0.0f;

	uint64_t texHandle_ = NULL;
	Sprite texSprite_;

protected:
	virtual void SpriteUpdate(const std::string& soundName, const std::string& name);
	virtual void SpriteDraw();

public:
	virtual ~ClearEffectState() { ; }

	void SetClearEffect(ClearEffect* clearEffect) { clearEffect_ = clearEffect; }

	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

public:
	static std::unique_ptr<ClearEffectState> GetState(const std::string& name);
};

//攻撃していない
class ClearEffectStateHyper : public ClearEffectState
{
	const float VOLUME_TMP_ = 2.0f;
	const std::string SOUND_NAME_ = "hyper.wav";

public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
};

//攻撃中
class ClearEffectStateHot : public ClearEffectState
{
private:
	const float VOLUME_TMP = 2.0f;
	const std::string SOUND_NAME_ = "hot.wav";

public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
};