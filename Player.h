#pragma once
#include "Sound.h"
#include "PlayerBulletManager.h"

class Player;

class PlayerAttackState
{
protected:
	Player* player = nullptr;

public:
	void SetPlayer(Player* player);
	virtual void Update(/*Tutorial* tutorial = nullptr*/) = 0;
	virtual void Draw(ViewMat& view, ProjectionMat& projection, Model* model, Model* modelAttack) = 0;
};


class Player : public Collider
{
private:
	//モデル
	Model* model_ = nullptr;
	Model* modelAttack = nullptr;

	//テクスチャハンドル
	uint32_t* textureHandle_;
	DebugText* debugText_ = nullptr;

	Vec3 velocity;

	//移動制限
	const float movingMax = 25.0f;

	//ジャンプ系
	const float gravityTmp = 0.05f;
	float velocityY = 0;
	const float jumpPowerTmp = 1.1f;
	const float velocityYMin = -5.0f;
	bool isJump = false;

	//地面にいるか
	bool isGround = false;

	float HPp;
	const float hptmp = 5;

	//攻撃の状態
	PlayerAttackState* state = nullptr;

	//無敵時間
	int dmageCoolTime = 0;
	const int dmageCoolTimeTmp = 90;

	const float scaleTmp = 3.0f;

	//hp
	Draw gaugeS;

	/*Tutorial* tutorial;*/

	//EffectManager* effectM_;
public:
	//sound
	Sound* audio = nullptr;
	KeyboardInput* input_ = nullptr;
	const Vec2 gaugeLength = { 1280,720 };
	Draw draw[10];
	PlayerBulletManager* playerBulletM;

	/*XINPUT_STATE joyState;
	XINPUT_STATE oldJoyState;*/


	void ChangeState(PlayerAttackState* state);

	void Initialize(Model* model, Model* modelAttack, PlayerBulletManager* playerBulletM/* EffectManager* effectM_,*/
	/*,Tutorial* tutorial = nullptr*/);
	void Update();
	void Draw( ViewMat& view,  ProjectionMat& projection);
	void DrawSprite();


	void SetWorldPos(const Vec3& pos) { worldTransform_.trans = pos; };
	float GetGravityTmp() { return gravityTmp; }
	float GetJumpPowerTmp() { return jumpPowerTmp; }
	float GetJumpPower() { return velocityY; }
	bool GetIsJump() { return isJump; }
	bool GetIsGround() { return isGround; }
	int GetHP() { return HPp; }

	void SetIsJump(bool isJump) { this->isJump = isJump; }
	void SetIsGround(bool isGround) { this->isGround = isGround; }
	void SetJumpPower(float jumpPower) { if (jumpPower > velocityYMin) this->velocityY = jumpPower; }
	void SetVelocity(Vec3 vec) { velocity = vec; }
	Vec3 GetVelocity() { return velocity; }

	//衝突を検出したら呼び出す（コールバック関数）
	//ダメージうけた時
	void OnCollision(Collider& collider)override;
	//敵踏んだ時
	void OnCollision2(Collider& collider)override;
};


//何もしていない
class NoAttackP : public PlayerAttackState
{
private:
	int count = 0;
	const int countMax = 20;

public:
	void Update(/*Tutorial* tutorial = nullptr*/);
	void Draw(ViewMat& view, ProjectionMat& projection, Model* model, Model* modelAttack);
};

//ジャンプ攻撃中
class JumpAttackP : public PlayerAttackState
{
private:
	int count = 0;
	const int countMax = 10;

public:
	void Update(/*Tutorial* tutorial = nullptr*/);
	void Draw(ViewMat& view, ProjectionMat& projection, Model* model, Model* modelAttack);
};