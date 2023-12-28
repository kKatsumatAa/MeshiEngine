/*
* @file ClearEffectState.h
* @brief クリア後の演出の状態
*/

#pragma once
#include"Util.h"
#include"Sprite.h"
#include<functional>


class ClearEffect;

class ClearEffectState
{
protected:
	const float TIME_RATE_MAX_ = 1.0f;

	const Vec4 SPRITE_COLOR_ = { 2.0f,2.0f,2.0f,1.0f };
	const Vec3 SPRITE_TRANS_ = { WindowsApp::WINDOW_WIDTH_ * 0.5f,WindowsApp::WINDOW_HEIGHT_ * 0.5f ,0 };
	const Vec2 SPRITE_ANCOR_UV_ = { 0.5f,0.5f };

	const int32_t TIMER_MAX_ = 70;
	const float MAX_SCALE_ = 1.5f;
	const float NORMAL_SCALE_ = 1.0f;

protected:
	//状態を使うインスタンスポインタ
	ClearEffect* clearEffect_ = nullptr;

	int32_t timer_ = 0;
	float timerRatio_ = 0;

	float scale_ = 0;

	float alpha_ = 0.0f;

	uint64_t texHandle_ = NULL;
	Sprite texSprite_;

protected:
	/// <summary>
	/// スプライトの更新処理
	/// </summary>
	/// <param name="soundName">音声の名前</param>
	/// <param name="nextStateName">次のステート名</param>
	virtual void SpriteUpdate(const std::string& soundName, const std::string& nextStateName);

	//スプライトの描画
	virtual void SpriteDraw();

public:
	//デストラクタ
	virtual ~ClearEffectState() { ; }

	/// <summary>
	/// クリア演出インスタンスのポインターセット
	/// </summary>
	/// <param name="clearEffect">クリア演出ポインター</param>
	void SetClearEffect(ClearEffect* clearEffect) { clearEffect_ = clearEffect; }

	//初期化
	virtual void Initialize() = 0;
	//更新
	virtual void Update() = 0;
	//描画
	virtual void Draw() = 0;

public:
	/// <summary>
	/// 名前でステートを取得
	/// </summary>
	/// <param name="name">ステート名</param>
	/// <returns>ステートのインスタンス</returns>
	static std::unique_ptr<ClearEffectState> GetState(const std::string& name);
};

//攻撃していない
class ClearEffectStateHyper : public ClearEffectState
{
	const float VOLUME_TMP_ = 0.7f;
	const std::string SOUND_NAME_ = "hyper.wav";

public:
	//初期化
	void Initialize() override;
	//更新
	void Update() override;
	//描画
	void Draw() override;
};

//攻撃中
class ClearEffectStateHot : public ClearEffectState
{
private:
	const float VOLUME_TMP = 0.7f;
	const std::string SOUND_NAME_ = "hot.wav";

public:
	//初期化
	void Initialize() override;
	//更新
	void Update() override;
	//描画
	void Draw() override;
};