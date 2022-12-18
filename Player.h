#pragma once
#include"Collider.h"
#include "Sound.h"

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
	//���f��
	Model* model_ = nullptr;
	Model* modelAttack = nullptr;

	//�e�N�X�`���n���h��
	uint32_t* textureHandle_;
	DebugText* debugText_ = nullptr;

	Vec3 velocity;

	//�W�����v�n
	const float gravityTmp = 0.05f;
	float velocityY = 0;
	const float jumpPowerTmp = 2.0f;
	const float velocityYMin = -5.0f;
	bool isJump = false;

	//�n�ʂɂ��邩
	bool isGround = false;

	float HPp;
	const float hptmp = 5;

	//�U���̏��
	PlayerAttackState* state = nullptr;

	//���G����
	int dmageCoolTime = 0;
	const int dmageCoolTimeTmp = 90;

	const float scaleTmp = 1.0f;

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

	/*XINPUT_STATE joyState;
	XINPUT_STATE oldJoyState;*/


	void ChangeState(PlayerAttackState* state);

	void Initialize(Model* model, Model* modelAttack/* EffectManager* effectM_,*/
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

	//�Փ˂����o������Ăяo���i�R�[���o�b�N�֐��j
	void OnCollision(Collider& collider)override;
	void OnCollision2(Collider& collider)override;
};


//�������Ă��Ȃ�
class NoAttack : public PlayerAttackState
{
private:
	int count = 0;
	const int countMax = 20;

public:
	void Update(/*Tutorial* tutorial = nullptr*/);
	void Draw(ViewMat& view, ProjectionMat& projection, Model* model, Model* modelAttack);
};

//�W�����v�U����
class JumpAttack : public PlayerAttackState
{
private:

public:
	void Update(/*Tutorial* tutorial = nullptr*/);
	void Draw(ViewMat& view, ProjectionMat& projection, Model* model, Model* modelAttack);
};